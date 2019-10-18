#include <register/hei_isolation_register.hpp>

namespace libhei
{

//------------------------------------------------------------------------------

bool IsolationRegister::analyze( const Chip & i_chip,
                                 AttentionType_t i_attnType,
                                 IsolationData & io_isoData ) const
{
    bool o_activeAttn = false; // Initially, assume no active attentions.

    // If this register/chip/attention type has already been analyzed, then
    // continuing would put us into an infinite loop.
    HEI_ASSERT( !addToIsolationStack(i_chip, i_attnType) );

    // A rule for i_attnType must exist.
    auto rule_itr = iv_rules.find( i_attnType );
    HEI_ASSERT( iv_rules.end() != rule_itr );

    // Get the returned BitString for this rule.
    const BitString * bs = rule_itr->second->getBitString( i_chip );

    // Ensure this BitString is not longer than the maximum bit field.
    HEI_ASSERT( bs->getBitLen() <= sizeof(RegisterBit_t) * 8 );

    // Find all active bits for this rule.
    for ( RegisterBit_t bit = 0; bit < bs->getBitLen(); bit++ )
    {
        // Continue to the next bit if not active.
        if ( !bs->isBitSet(bit) ) continue;

        // At least one active bit was found.
        o_activeAttn = true;

        // Determine if this attention originated from another register or if it
        // is a leaf in the isolation tree.
        auto child_itr = iv_isolationTree.find( bit );
        if ( iv_isolationTree.end() != child_itr )
        {
            // This bit was driven from an attention from another register.
            // Continue down the isolation tree to look for more attentions.
            bool attnFound = child_itr->second->analyze( i_chip, i_attnType,
                                                         io_isoData );
            if ( !attnFound )
            {
                // Something went wrong. There should have been an active
                // attention. It's possible there is a bug in the Chip Data
                // File. Or, it is also possible some other piece of code is
                // clearing the attention before this code is able to analyze
                // it. Another possibility is that the hardware it not behaving
                // as expected. Since we really don't know what happened, we
                // should not assert. Instead, add this bit's signature to
                // io_isoData. If there are no other active attentions, the user
                // application could use this signature to help determine, and
                // circumvent, the isolation problem.
                io_isoData.addSignature( Signature { i_chip, iv_hwReg.getId(),
                                                     iv_hwReg.getInstance(),
                                                     bit, i_attnType } );
            }
        }
        else
        {
            // We have reached a leaf in the isolation tree. Add this bit's
            // signature to io_isoData.
            io_isoData.addSignature( Signature { i_chip, iv_hwReg.getId(),
                                                 iv_hwReg.getInstance(),
                                                 bit, i_attnType } );
        }
    }

    return o_activeAttn;
}

//------------------------------------------------------------------------------

void IsolationRegister::addRule( AttentionType_t i_attnType,
                                 const Register * i_rule )
{
    // A rule for this attention type should not already exist.
    HEI_ASSERT( iv_rules.end() == iv_rules.find(i_attnType) );

    // The rule should not be null.
    HEI_ASSERT( nullptr != i_rule );

    // Add the new rule.
    iv_rules[i_attnType] = i_rule;
}

//------------------------------------------------------------------------------

void IsolationRegister::addChild( uint8_t i_bit,
                                  const IsolationRegister * i_child )
{
    // An entry for this bit should not already exist.
    HEI_ASSERT( iv_isolationTree.end() == iv_isolationTree.find(i_bit) );

    // The child register should not be null.
    HEI_ASSERT( nullptr != i_child );

    // Add the new rule.
    iv_isolationTree[i_bit] = i_child;
}

//------------------------------------------------------------------------------

std::map< Chip, std::map< AttentionType_t, const IsolationRegister * > >
                                    IsolationRegister::cv_isolationStack {};

//------------------------------------------------------------------------------

bool IsolationRegister::addToIsolationStack( const Chip & i_chip,
                                             AttentionType_t i_attnType ) const
{
    // Determine if an entry exists for this chip and attention type.
    auto chip_itr = cv_isolationStack.find( i_chip );
    if ( (cv_isolationStack.end() != chip_itr                          ) &&
         (chip_itr->second.end()  != chip_itr->second.find(i_attnType)) )
    {
        return true; // An entry already exists.
    }
    else
    {
        // An entry does not exist, so add it.
        cv_isolationStack[i_chip][i_attnType] = this;
        return false;
    }
}

//------------------------------------------------------------------------------

} // end namespace libhei

