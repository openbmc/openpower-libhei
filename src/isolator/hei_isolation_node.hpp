#pragma once

#include <hei_isolation_data.hpp>
#include <register/hei_hardware_register.hpp>
#include <util/hei_includes.hpp>

namespace libhei
{

/**
 * @brief This class contains the isolation rules and bit definition for a node
 *        in a chip's error reporting structure.
 *
 * These objects are linked together to form a tree with a single root node. Any
 * active bits found in a node will either indicate an active attention or that
 * the attention originated in a child node.
 *
 * The primary function of this class is analyze(), which will do a depth-first
 * search of the tree to find all active attentions and add their signatures to
 * the returned isolation data.
 *
 * The tree structure is built from information in the Chip Data Files. It is
 * possible that the tree could be built with loop in the isolation. This would
 * be bug in the Chip Data Files. This class will keep track of all nodes that
 * have been analyzed to prevent cyclic isolation (an infinite loop).
 *
 * Each node instance will represent a register, or set of registers, that can
 * be configured to represent one or more attention types. These configuration
 * rules are a combination of hardware register objects and operator registers
 * objects to define rules like "REG & ~MASK & CNFG", which reads "return all
 * bits in REG that are not in MASK and set in CNFG". See the definition of the
 * Register class for details on how this works.
 *
 * This class cannot be added to the flyweights. There is no way to easily
 * distinguish differences between nodes on different chips without comparing
 * all of the capture registers, rules, and child nodes. Instead, the shared
 * pointers will be stored in the isolation chip, which will ensure there isn't
 * an attempt to add two nodes with the same ID and instance.
 */
class IsolationNode
{
  public: // Aliases
    using Ptr = std::shared_ptr<IsolationNode>;
    using ConstPtr = std::shared_ptr<const IsolationNode>;

    using Key = std::pair<NodeId_t, Instance_t>;

  public: // Constructors, destructor, assignment
    /**
     * @brief Constructor from components.
     * @param i_id       Unique ID for all instances of this node.
     * @param i_instance Instance of this node.
     */
    IsolationNode(NodeId_t i_id, Instance_t i_instance,
                  RegisterType_t i_regType) :
        iv_id(i_id), iv_instance(i_instance), iv_regType(i_regType)
    {}

    /** @brief Destructor. */
    ~IsolationNode() = default;

    /** @brief Copy constructor. */
    IsolationNode(const IsolationNode&) = delete;

    /** @brief Assignment operator. */
    IsolationNode& operator=(const IsolationNode&) = delete;

  private: // Instance variables
    /** The unique ID for all instances of this node. */
    const NodeId_t iv_id;

    /**
     * A node may have multiple instances. All of which will have the same ID.
     * This variable is used to distinguish between each instance of the node.
     */
    const Instance_t iv_instance;

    /**
     * A registers referenced by this node's rules must be of this type. No
     * mixing of register types is allowed because comparing different sized
     * registers is undefined behavior. Note that it is possible to have capture
     * registers of mixed types.
     */
    const RegisterType_t iv_regType;

    /**
     * The lists of register to capture and add to the log for additional
     * debugging. The lists are indexed in a map where the key is a bit
     * position. All registers that should be captured by default when
     * isolating to this node will have a bit position of `MAX_BIT_POSITION`.
     * Otherwise, any other list targeted for a specific bit will only be
     * captured if there is an active attention on that bit.
     */
    std::map<BitPosition_t, std::vector<HardwareRegister::ConstPtr>> iv_capRegs;

    /**
     * This register could report multiple types of attentions. We can use a
     * register 'rule' (value) to find any active attentions for each attention
     * type (key). A 'rule', like "register & ~mask", is a combination of
     * HardwareRegister objects and virtual operator registers (all children
     * of the Register class). Note that all registers referenced by these rules
     * must be the same type as iv_regType.
     */
    std::map<AttentionType_t, const Register::ConstPtr> iv_rules;

    /**
     * Each bit (key) in this map indicates that an attention was driven from
     * another register (value).
     */
    std::map<BitPosition_t, const ConstPtr> iv_children;

    /**
     * This map is used to store the write operation rules for the isolation
     * node as defined in the Chip Data Files.
     */
    std::map<OpRuleName_t, std::pair<OpRuleType_t, RegisterId_t>> iv_op_rules;

  public: // Member functions
    /**
     * @brief  Finds all active attentions on this node. If an active bit is a
     *         leaf in the isolation tree, the bit's signature is added to the
     *         isolation data. Otherwise, this function is recursively called
     *         to analyze the child node that is driving the attention in this
     *         node.
     * @param  i_chip     The target chip for isolation.
     * @param  i_attnType The target attention type to analyze on this register.
     *                    Will assert a rule must exist for this attention type.
     * @param  io_isoData The isolation data returned back to the user
     *                    application.
     * @return True, if any active attentions found on this register.
     *         False, otherwise.
     */
    bool analyze(const Chip& i_chip, AttentionType_t i_attnType,
                 IsolationData& io_isoData) const;

    /**
     * @brief Adds a hardware register to the list of registers that will be
     *        captured for additional debugging. See iv_capRegs for details.
     *
     * This is only intended to be used during initialization of the isolator.
     * Duplicate registers will be ignored.
     *
     * @param i_hwReg The target hardware register.
     * @param i_bit   If specified, the given register should only be captured
     *                when there is an active attention on the given bit. If
     *                omitted, the given register will be captured any time
     *                this isolation node is analyzed.
     */
    void addCaptureRegister(HardwareRegister::ConstPtr i_hwReg,
                            BitPosition_t i_bit = MAX_BIT_POSITION);

    /**
     * @brief Adds a register rule for the given attention type. See iv_rules
     *        for details.
     *
     * This is only intended to be used during initialization of the isolator.
     * Will assert that a rule has not already been defined for this type.
     *
     * @param i_attnType The target attention type.
     * @param i_rule The rule for this attention type.
     */
    void addRule(AttentionType_t i_attnType, Register::ConstPtr i_rule);

    /**
     * @brief Adds a child node to analyze for the given bit position in this
     *        node. See iv_children for details.
     *
     * This is only intended to be used during initialization of the isolator.
     * Will assert that nothing has already been defined for this bit.
     *
     * @param i_bit The target bit on this node.
     * @param i_child The child node to analyze for the given bit.
     */
    void addChild(BitPosition_t i_bit, ConstPtr i_child);

    /**
     * @brief Adds a new write operation for the isolation node.
     *
     * This is only intended to be used during initialization of the isolator.
     * Will assert that nothing has already been defined for this type.
     *
     * @param i_opName The name of the operation.
     * @param i_opType The type of the operation.
     * @param i_regId The ID of the register to be written.
     */
    void addOpRule(OpRuleName_t i_opName, OpRuleType_t i_opType,
                   RegisterId_t i_regId);

    /**
     * @brief Returns a write operation for the isolation node based on the
     *        input operation name.
     *
     * @param i_name The name of the operation.
     * @return The operation type and reg ID of the operation rule in a pair.
     */
    std::pair<OpRuleType_t, RegisterId_t> getOpRule(OpRuleName_t i_name) const;

    /**
     * @brief Returns whether the write operation rule exists for the node
     *
     * @param i_name The name of the operation.
     * @return True if the operation exists, else false.
     */
    bool doesOpExist(OpRuleName_t i_name) const;

    /** @return The node ID. */
    NodeId_t getId() const
    {
        return iv_id;
    }

    /** @return The node instance. */
    Instance_t getInstance() const
    {
        return iv_instance;
    }

    /** @return The node/instance key. */
    Key getKey() const
    {
        return {iv_id, iv_instance};
    }

    /** @return This node's register type.. */
    RegisterType_t getRegisterType() const
    {
        return iv_regType;
    }

  private: // Member functions
    /**
     * @param  i_chip     The target chip for isolation.
     * @param  io_isoData The isolation data returned back to the user
     *                    application.
     * @param  i_bit      If specified, only the registers specifically
     *                    targeted for the given bit are captured. If omitted,
     *                    the default list of registers for this isolation node
     *                    will be captured.
     */
    void captureRegisters(const Chip& i_chip, IsolationData& io_isoData,
                          BitPosition_t i_bit = MAX_BIT_POSITION) const;

  private: // Isolation stack and supporting functions.
    /** When analyze() is called at the tree root, all recursive calls to
     *  analyze() will target the same chip and attention type. So we only need
     *  to keep track of the nodes that have been analyzed to avoid cyclic
     *  isolation (an infinite loop). In fact, we only need to keep track of the
     *  nodes directly from this node to the root node. As long as this node
     *  does not already exist in the list, we can be sure there will not be a
     *  loop. So the list can be treated as a stack. When analyze() is called on
     *  a node, that node is pushed to the top of the stack (as long as it
     *  doesn't already exist in the stack). Then, just before analyze() exits,
     *  this node can be popped off the top of the stack. Once all the recursive
     *  calls have returned back to the root node the stack should be empty.
     */
    static std::vector<const IsolationNode*> cv_isolationStack;

    /**
     * @brief Pushes this node to the top of the stack. Will assert that this
     *        node does not already exist in cv_isolationStack.
     */
    void pushIsolationStack() const;

    /** @brief Pops the top node off of cv_isolationStack. */
    void popIsolationStack() const
    {
        cv_isolationStack.pop_back();
    }
};

} // end namespace libhei
