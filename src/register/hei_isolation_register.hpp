#pragma once

#include <hei_includes.hpp>
#include <hei_isolation_data.hpp>
#include <register/hei_register.hpp>
#include <register/hei_hardware_register.hpp>
#include <util/hei_bit_string.hpp>
#include <util/hei_flyweight.hpp>

namespace libhei
{

/**
 * @brief This class contains the isolation rules and bit definition of a
 *        HardwareRegister used for error isolation.
 *
 * These objects are built like a tree. Any active bits in a register will
 * either be a true active attention (leaf) or indicate one or more active
 * attentions occurred in a child register (node).
 *
 * The primary function of this class is analyze(), which will do a depth-first
 * search of the tree to find all leaves and add their signatures to the
 * returned isolation data.
 *
 * It is possible that the isolation tree can be built in such a way that
 * isolation may loop. Therefore, This class will keep track of which registers
 * have been analyzed to avoid cyclic isolation (an infinite loop). This will
 * require the main isolation code to call flushIsolationStack() to ensure a
 * clean state before starting isolation.
 *
 * Each isolation register will have a rule for each supported attention type.
 * These rules are a combination of HardwareRegisters and operator registers to
 * define rules like "REG & ~MASK & CNFG", which reads "return all bits in REG
 * that are not in MASK and set in CNFG". See the definition of the Register
 * class for details on how this works.
 */
class IsolationRegister
{
  public: // Constructors, destructor, assignment

    /**
     * @brief Constructor from components.
     * @param i_hwReg A reference to the HardwareRegister targeted for
     *                isolation.
     */
    explicit IsolationRegister( const HardwareRegister & i_hwReg ) :
        iv_hwReg( i_hwReg )
    {}

    /** @brief Destructor. */
    ~IsolationRegister() = default;

  private:

    // This is needed to allow the flyweights to use the copy constructor, but
    // not allow it to be used in general.
    friend class Flyweight<IsolationRegister>;

    /**
     * @brief Copy constructor.
     *
     * Needed by Flyweight class, but should not be allowed in general.
     */
    IsolationRegister( const IsolationRegister & ) = default;

    /**
     * @brief Explicitly disables assignment operator.
     *
     * This is redundant since the compilier will implicitly delete this because
     * of the constant instance variables, but helps communicate it is not
     * allowed.
     */
    IsolationRegister & operator=( const IsolationRegister & ) = delete;

  private: // Instance variables

    /**
     * This is a reference to the HardwareRegister targeted for isolation by
     * this instance of the class. The reference is required to maintain
     * polymorphism.
     */
    const HardwareRegister & iv_hwReg;

    /**
     * This register could report multiple types of attentions. We can use a
     * register 'rule' (value) to find any active attentions for each attention
     * type (key). A 'rule', like "register & ~mask", is a combination of
     * HardwareRegister objects and virtual operator registers (all children
     * of the Register class).
     */
    std::map<AttentionType_t, const Register *> iv_rules;

    /**
     * Each bit (key) in this map indicates that an attention was driven from
     * another register (value).
     */
    std::map<RegisterBit_t, const IsolationRegister *> iv_isolationTree;

  public: // Member functions

    /**
     * @brief  Finds all active attentions on this register. If an active bit is
     *         a leaf in the isolation tree, the bit's signature is added to the
     *         isolation data. Otherwise, this function is recursively called
     *         to analyze the child register that is driving the attention in
     *         this register.
     * @param  i_chip     The target chip for isolation.
     * @param  i_attnType The target attention type to analyze on this register.
     *                    Will assert a rule must exist for this attention type.
     * @param  io_isoData The isolation data returned back to the user
     *                    application.
     * @return True, if any active attentions found on this register.
     *         False, otherwise.
     */
    bool analyze( const Chip & i_chip, AttentionType_t i_attnType,
                  IsolationData & io_isoData ) const;

    // TODO: The next two functions are only intended to be used during
    //       initialization of the isolator. Consider, making them private and
    //       make the Chip Data File code friends of this class. So that it has
    //       access to these init functions.

    /**
     * @brief Adds a register rule for the given attention type. See iv_rules
     *        for details.
     *
     * This is only intended to be used during initialization of the isolator.
     * Will assert that nothing has already been defined for this rule.
     *
     * @param The target attention type.
     * @param The rule for this attention type.
     */
    void addRule( AttentionType_t i_attnType, const Register * i_rule );

    /**
     * @brief Adds a child register to analyze for the given bit in this
     *        register. See iv_isolationTree for details.
     *
     * This is only intended to be used during initialization of the isolator.
     * Will assert that nothing has already been defined for this bit.
     *
     * @param The target bit on this register.
     * @param The child register to analyze for the given bit.
     */
    void addChild( RegisterBit_t i_bit, const IsolationRegister * i_child );

  public: // Operators

    /** @brief Equals operator. */
    bool operator==( const IsolationRegister & i_r ) const
    {
        // iv_hwReg should be unique per IsolationRegister.
        return ( iv_hwReg == i_r.iv_hwReg );
    }

    /** @brief Less than operator. */
    bool operator<( const IsolationRegister & i_r ) const
    {
        // iv_hwReg should be unique per IsolationRegister.
        return ( iv_hwReg < i_r.iv_hwReg );
    }

  private:

    /** It is possible that the isolation tree can be built in such a way that
     *  isolation may loop. Therefore, this is used to keep track of which
     *  registers have been analyzed to avoid cyclic isolation (an infinite
     *  loop). */
    static std::map< Chip,
                     std::map< AttentionType_t,
                               const IsolationRegister * > > cv_isolationStack;

    /**
     * @brief  First, checks if this register/chip/attention already exists in
     *         the isolation stack. If not, adds it to the stack.
     * @param  i_chip     The target chip for isolation.
     * @param  i_attnType The target attention type to analyze on this register.
     * @return True, if this register/chip/attention already exists in the
     *         isolation stack. False, otherwise.
     */
    bool addToIsolationStack( const Chip & i_chip,
                              AttentionType_t i_attnType ) const;

  public:

    /**
     * @brief Clears the isolation stack. This should be called at the
     *        beginning of the main isolation function to ensure a clean state.
     */
    static void flushIsolationStack() { cv_isolationStack.clear(); }

};

} // end namespace libhei

