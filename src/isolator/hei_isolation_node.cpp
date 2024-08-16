#include <isolator/hei_isolation_node.hpp>
#include <util/hei_bit_string.hpp>

namespace libhei
{

//------------------------------------------------------------------------------

bool IsolationNode::analyze(const Chip& i_chip, AttentionType_t i_attnType,
                            IsolationData& io_isoData) const
{
    bool o_activeAttn = false; // Initially, assume no active attentions.

    // Keep track of nodes that have been analyzed to avoid cyclic isolation.
    pushIsolationStack();

    // Capture default set of registers for this node.
    captureRegisters(i_chip, io_isoData);

    // Get the rule for this attention type.
    auto rule_itr = iv_rules.find(i_attnType);

    // It is possible that a rule does not exist. The likely scenario is that
    // this node is intended to only gather FFDC for a specific bit in the
    // parent node.
    if (iv_rules.end() != rule_itr)
    {
        // Get the returned BitString for this rule.
        const BitString* bs = rule_itr->second->getBitString(i_chip);

        // Ensure this BitString is not longer than the maximum bit field.
        HEI_ASSERT(bs->getBitLen() <= (1 << (sizeof(BitPosition_t) * 8)));

        // Find all active bits for this rule.
        for (BitPosition_t bit = 0; bit < bs->getBitLen(); bit++)
        {
            // Continue to the next bit if not active.
            if (!bs->isBitSet(bit))
                continue;

            // At least one active bit was found.
            o_activeAttn = true;

            // Capture registers specific to this isolation bit.
            captureRegisters(i_chip, io_isoData, bit);

            // Determine if this attention originated from another register or
            // if it is a leaf in the isolation tree.
            auto child_itr = iv_children.find(bit);
            if (iv_children.end() != child_itr)
            {
                // This bit was driven from an attention from another register.
                // Continue down the isolation tree to look for more attentions.
                bool attnFound =
                    child_itr->second->analyze(i_chip, i_attnType, io_isoData);
                if (!attnFound)
                {
                    // It is possible the child node is only intended for FFDC.
                    // See comment near the check for a valid rule above.
                    // Otherwise, it is possible something went wrong. If there
                    // should have been an active attention, it's possible there
                    // is a bug in the Chip Data File. Or, it is also possible
                    // some other piece of code is clearing the attention before
                    // this code is able to analyze it. Another possibility is
                    // that the hardware it not behaving as expected. Since we
                    // really don't know what happened, we should not assert.
                    // Instead, add this bit's signature to io_isoData. If there
                    // are no other active attentions, the user application
                    // could use this signature to help determine, and
                    // circumvent, the isolation problem.
                    io_isoData.addSignature(
                        Signature{i_chip, iv_id, iv_instance, bit, i_attnType});
                }
            }
            else
            {
                // We have reached a leaf in the isolation tree. Add this bit's
                // signature to io_isoData.
                io_isoData.addSignature(
                    Signature{i_chip, iv_id, iv_instance, bit, i_attnType});
            }
        }
    }

    // Analysis is complete on this node. So remove it from cv_isolationStack.
    popIsolationStack();

    return o_activeAttn;
}

//------------------------------------------------------------------------------

void IsolationNode::addCaptureRegister(HardwareRegister::ConstPtr i_hwReg,
                                       BitPosition_t i_bit)
{
    HEI_ASSERT(i_hwReg); // should not be null

    // Check the bit range.
    if (MAX_BIT_POSITION != i_bit)
    {
        if (REG_TYPE_SCOM == iv_regType || REG_TYPE_ID_SCOM == iv_regType)
        {
            HEI_ASSERT(i_bit < 64);
        }
        else
        {
            HEI_ASSERT(false); // register type unsupported
        }
    }

    // Add this capture register only if it does not already exist in the list.
    auto itr = iv_capRegs.find(i_bit);
    if (iv_capRegs.end() == itr ||
        itr->second.end() ==
            std::find(itr->second.begin(), itr->second.end(), i_hwReg))
    {
        iv_capRegs[i_bit].push_back(i_hwReg);
    }
}

//------------------------------------------------------------------------------

void IsolationNode::addRule(AttentionType_t i_attnType,
                            Register::ConstPtr i_rule)
{
    HEI_ASSERT(i_rule); // should not be null

    auto ret = iv_rules.emplace(i_attnType, i_rule);

    // If an entry already existed, it must point to the same object.
    HEI_ASSERT(ret.second || (ret.first->second == i_rule));
}

//------------------------------------------------------------------------------

void IsolationNode::addChild(uint8_t i_bit, ConstPtr i_child)
{
    HEI_ASSERT(i_child); // should not be null

    auto ret = iv_children.emplace(i_bit, i_child);

    // If an entry already existed, it must point to the same object.
    HEI_ASSERT(ret.second || (ret.first->second == i_child));
}

//------------------------------------------------------------------------------

void IsolationNode::addOpRule(OpRuleName_t i_opName, OpRuleType_t i_opType,
                              RegisterId_t i_regId)
{
    std::pair<OpRuleType_t, RegisterId_t> tmpPair = {i_opType, i_regId};
    auto ret = iv_op_rules.emplace(i_opName, tmpPair);

    HEI_ASSERT(ret.second || ret.first->second == tmpPair);
}

//------------------------------------------------------------------------------

std::pair<OpRuleType_t, RegisterId_t>
    IsolationNode::getOpRule(OpRuleName_t i_name) const
{
    return iv_op_rules.at(i_name);
}

//------------------------------------------------------------------------------

bool IsolationNode::doesOpExist(OpRuleName_t i_name) const
{
    return (0 != iv_op_rules.count(i_name));
}

//------------------------------------------------------------------------------

std::vector<const IsolationNode*> IsolationNode::cv_isolationStack{};

//------------------------------------------------------------------------------

void IsolationNode::pushIsolationStack() const
{
    // Ensure this node does not already exist in cv_isolationStack.
    auto itr =
        std::find(cv_isolationStack.begin(), cv_isolationStack.end(), this);
    HEI_ASSERT(cv_isolationStack.end() == itr);

    // Push to node to the stack.
    cv_isolationStack.push_back(this);
}

//------------------------------------------------------------------------------

void IsolationNode::captureRegisters(
    const Chip& i_chip, IsolationData& io_isoData, BitPosition_t i_bit) const
{
    auto itr = iv_capRegs.find(i_bit);
    if (iv_capRegs.end() != itr)
    {
        // Capture all registers for this node.
        for (const auto& hwReg : itr->second)
        {
            // Read the register (adds BitString to register cache).
            if (hwReg->read(i_chip))
            {
                // The register read failed.
                // TODO: Would be nice to add SCOM errors to the log just in
                // case
                //       traces are not available.
                // TODO: This trace could be redundant with the user
                // application,
                //       which will have more information on the actual chip
                //       that failed anyway. Leaving it commented out for now
                //       until the SCOM errors are added to the log.
                // HEI_ERR("register read failed on chip type=0x%0" PRIx32
                //         "address=0x%0" PRIx64,
                //         i_chip.getType(), hwReg->getAddress());
            }
            else
            {
                // Add to the FFDC.
                io_isoData.addRegister(i_chip, hwReg->getId(),
                                       hwReg->getInstance(),
                                       hwReg->getBitString(i_chip));
            }
        }
    }
}

//------------------------------------------------------------------------------

} // end namespace libhei
