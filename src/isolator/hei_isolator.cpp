
#include <isolator/hei_isolation_node.hpp>
#include <isolator/hei_isolator.hpp>
#include <register/hei_operator_register.hpp>
#include <register/hei_scom_register.hpp>
#include <util/hei_flyweight.hpp>
#include <util/hei_includes.hpp>

namespace libhei
{

//------------------------------------------------------------------------------

// Definitions for the interfaces defined in hei_main.hpp.

void initialize(void* i_buffer, size_t i_bufferSize)
{
    Isolator::getSingleton().initialize(i_buffer, i_bufferSize);
}

void uninitialize()
{
    Isolator::getSingleton().uninitialize();
}

void isolate(const std::vector<Chip>& i_chipList, IsolationData& o_isoData)
{
    Isolator::getSingleton().isolate(i_chipList, o_isoData);
}

//------------------------------------------------------------------------------

void Isolator::uninitialize()
{
    // Clear all of the isolation objects.
    iv_isoChips.clear();

    // Must flush the hardware register cache before deleting any
    // HardwareRegister objects.
    HardwareRegister::flushAll();

    // Clear the operator register flyweights.
    Flyweight<const ConstantRegister>::getSingleton().clear();
    Flyweight<const AndRegister>::getSingleton().clear();
    Flyweight<const OrRegister>::getSingleton().clear();
    Flyweight<const NotRegister>::getSingleton().clear();
    Flyweight<const LeftShiftRegister>::getSingleton().clear();
    Flyweight<const RightShiftRegister>::getSingleton().clear();

    // Remove all of the HardwareRegister objects stored in the flyweights.
    Flyweight<const ScomRegister>::getSingleton().clear();
    Flyweight<const IdScomRegister>::getSingleton().clear();
}

void Isolator::isolate(const std::vector<Chip>& i_chipList,
                       IsolationData& o_isoData) const
{
    // Flush the isolation data to ensure a clean slate.
    o_isoData.flush();

    // Flush the hardware register cache to avoid using stale data.
    HardwareRegister::flushAll();

    // Analyze active error on each chip.
    for (const auto& chip : i_chipList)
    {
        // Isolation objects for this chip's type must exist.
        const auto& itr = iv_isoChips.find(chip.getType());
        HEI_ASSERT(iv_isoChips.end() != itr);

        // Analyze this chip.
        itr->second->analyze(chip, o_isoData);
    }
}

//------------------------------------------------------------------------------

#ifdef __HEI_ENABLE_HW_WRITE

bool __atomicOr(Signature i_sig, HardwareRegister::ConstPtr i_hwReg)
{
    // Input register is for an atomic OR register used for setting a bit. Get
    // the bit in the input signature and write that bit to the register.
    i_hwReg->clearAllBits(i_sig.getChip());
    i_hwReg->setBit(i_sig.getChip(), i_sig.getBit());

    return i_hwReg->write(i_sig.getChip());
}

bool __atomicAnd(Signature i_sig, HardwareRegister::ConstPtr i_hwReg)
{
    // Input register is for an atomic AND register used for clearing a bit. Get
    // the bit in the input signature and write that bit to the register.
    i_hwReg->setAllBits(i_sig.getChip());
    i_hwReg->clearBit(i_sig.getChip(), i_sig.getBit());

    return i_hwReg->write(i_sig.getChip());
}

bool __readSetWrite(Signature i_sig, HardwareRegister::ConstPtr i_hwReg)
{
    // Perform a read, modify, write to set a bit. Get the bit from the input
    // signature.
    if (i_hwReg->read(i_sig.getChip(), true))
    {
        HEI_ERR("Failed to read reg ID 0x%04x", i_hwReg->getId());
        return true;
    }
    i_hwReg->setBit(i_sig.getChip(), i_sig.getBit());

    return i_hwReg->write(i_sig.getChip());
}

bool __readClearWrite(Signature i_sig, HardwareRegister::ConstPtr i_hwReg)
{
    // Perform a read, modify, write to clear a bit. Get the bit from the input
    // signature.
    if (i_hwReg->read(i_sig.getChip(), true))
    {
        HEI_ERR("Failed to read reg ID 0x%04x", i_hwReg->getId());
        return true;
    }
    i_hwReg->clearBit(i_sig.getChip(), i_sig.getBit());

    return i_hwReg->write(i_sig.getChip());
}

void Isolator::performWriteOp(OpRuleName_t i_op, Signature i_sig)
{
    // Use the signature to determine the relevant isolation node.
    IsolationNode::Key nodeKey = {i_sig.getId(), i_sig.getInstance()};
    IsolationChip::ConstPtr isoChip = iv_isoChips.at(i_sig.getChip().getType());
    IsolationNode::ConstPtr node = isoChip->getIsolationNode(nodeKey);

    // If the operation name does not exist for the node, print an error
    // message and return.
    if (!node->doesOpExist(i_op))
    {
        HEI_ERR("Operation rule %d does not exist for node 0x%04x", i_op,
                node->getId());
        return;
    }

    // Get the write operation defined in the node.
    std::pair<OpRuleType_t, RegisterId_t> op = node->getOpRule(i_op);

    // Get the relevant hardware register from the isolation chip. The instance
    // of the register should match the instance of the signature.
    HardwareRegister::Key regKey = {op.second, i_sig.getInstance()};
    HardwareRegister::ConstPtr hwReg = isoChip->getHardwareRegister(regKey);

    // Perform write operation dependent on the operation type.
    switch (op.first)
    {
        case ATOMIC_OR:
        {
            if (__atomicOr(i_sig, hwReg))
            {
                HEI_ERR("Failed performing ATOMIC_OR write operation %d on "
                        "node 0x%04x",
                        i_op, node->getId());
            }
            break;
        }
        case ATOMIC_AND:
        {
            if (__atomicAnd(i_sig, hwReg))
            {
                HEI_ERR("Failed performing ATOMIC_AND write operation %d on "
                        "node 0x%04x",
                        i_op, node->getId());
            }
            break;
        }
        case READ_SET_WRITE:
        {
            if (__readSetWrite(i_sig, hwReg))
            {
                HEI_ERR("Failed performing READ_SET_WRITE write operation %d "
                        "on node 0x%04x",
                        i_op, node->getId());
            }
            break;
        }
        case READ_CLEAR_WRITE:
        {
            if (__readClearWrite(i_sig, hwReg))
            {
                HEI_ERR("Failed performing READ_CLEAR_WRITE write operation %d "
                        "on node 0x%04x",
                        i_op, node->getId());
            }
            break;
        }
        default:
        {
            HEI_ERR("Invalid operation type %d for op %d on node 0x%04x",
                    op.first, i_op, node->getId());
        }
    }

    // Flush the affected register from the cache so it is re-read from hardware
    // next time it is read.
    hwReg->flush(i_sig.getChip());
}

#endif

} // end namespace libhei
