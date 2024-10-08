#include <chip_data/hei_chip_data.hpp>
#include <chip_data/hei_chip_data_stream.hpp>
#include <register/hei_operator_register.hpp>
#include <register/hei_scom_register.hpp>

namespace libhei
{

//------------------------------------------------------------------------------

using SectionKeyword_t = uint32_t;

constexpr SectionKeyword_t KW_REGS = 0x52454753; // "REGS" ASCII
constexpr SectionKeyword_t KW_NODE = 0x4e4f4445; // "NODE" ASCII
constexpr SectionKeyword_t KW_ROOT = 0x524f4f54; // "ROOT" ASCII

using Version_t = uint8_t;

constexpr Version_t VERSION_1 = 0x01;
constexpr Version_t VERSION_2 = 0x02;
constexpr Version_t VERSION_3 = 0x03;

//------------------------------------------------------------------------------

void __readRegister(ChipDataStream& io_stream, IsolationChip::Ptr& io_isoChip)
{
    // Read the register metadata.
    RegisterId_t id;
    RegisterType_t type;
    RegisterAttributeFlags_t attr;
    Instance_t numInsts;
    io_stream >> id >> type >> attr >> numInsts;

    // Must have at least one instance.
    HEI_ASSERT(0 != numInsts);

    for (unsigned int i = 0; i < numInsts; i++)
    {
        // Read the register instance metadata.
        Instance_t inst;
        io_stream >> inst;

        // The address size is dependent on the register type.
        if (REG_TYPE_SCOM == type)
        {
            uint32_t addr; // 4-byte address.
            io_stream >> addr;

            // Get this register from the flyweight factory.
            auto& factory = Flyweight<const ScomRegister>::getSingleton();
            auto hwReg = factory.get(id, inst, attr, addr);

            // Add this register to the isolation chip.
            io_isoChip->addHardwareRegister(hwReg);
        }
        else if (REG_TYPE_ID_SCOM == type)
        {
            uint64_t addr; // 8-byte address.
            io_stream >> addr;

            // Get this register from the flyweight factory.
            auto& factory = Flyweight<const IdScomRegister>::getSingleton();
            auto hwReg = factory.get(id, inst, attr, addr);

            // Add this register to the isolation chip.
            io_isoChip->addHardwareRegister(hwReg);
        }
        else
        {
            HEI_ASSERT(false); // Register type unsupported.
        }
    }
}

//------------------------------------------------------------------------------

Register::ConstPtr __readExpr(ChipDataStream& io_stream,
                              const IsolationChip::Ptr& i_isoChip,
                              IsolationNode::Ptr& io_isoNode)
{
    Register::ConstPtr expr{};

    uint8_t exprType;
    io_stream >> exprType;
    switch (exprType)
    {
        case 0x01: // register reference
        {
            RegisterId_t regId;
            Instance_t regInst;
            io_stream >> regId >> regInst;

            // Find the hardware register that is stored in this isolation chip
            // and add it to the list of capture registers. This ensures all
            // registers referenced in the rules are are captured by default.
            // Note that this will assert that the target register must exist in
            // the isolation chip.
            auto hwReg = i_isoChip->getHardwareRegister({regId, regInst});

            // Add the register to the isolation node.
            io_isoNode->addCaptureRegister(hwReg);

            // Simply return this register.
            expr = hwReg;

            break;
        }
        case 0x02: // integer constant
        {
            auto& factory = Flyweight<const ConstantRegister>::getSingleton();

            if (REG_TYPE_SCOM == io_isoNode->getRegisterType() ||
                REG_TYPE_ID_SCOM == io_isoNode->getRegisterType())
            {
                uint64_t constant; // 8-byte value
                io_stream >> constant;

                // Create the constant register and put it in the flyweights.
                expr = factory.get(constant);
            }
            else
            {
                HEI_ASSERT(false); // register type unsupported
            }

            break;
        }
        case 0x10: // AND operation
        {
            auto& factory = Flyweight<const AndRegister>::getSingleton();

            uint8_t numSubExpr;
            io_stream >> numSubExpr;

            HEI_ASSERT(2 <= numSubExpr); // must be at least two

            // Read the first two sub-expressions.
            auto e1 = __readExpr(io_stream, i_isoChip, io_isoNode);
            auto e2 = __readExpr(io_stream, i_isoChip, io_isoNode);
            HEI_ASSERT(e1 && e2); // Cannot be null

            // Create the AND register and put it in the flyweights.
            expr = factory.get(e1, e2);

            // Iterate any remaining expressions.
            for (uint8_t i = 2; i < numSubExpr; i++)
            {
                // Read the next sub-expressions.
                e2 = __readExpr(io_stream, i_isoChip, io_isoNode);
                HEI_ASSERT(e2); // Cannot be null

                // Create the AND register and put it in the flyweights.
                expr = factory.get(expr, e2);
            }

            break;
        }
        case 0x11: // OR operation
        {
            auto& factory = Flyweight<const OrRegister>::getSingleton();

            uint8_t numSubExpr;
            io_stream >> numSubExpr;

            HEI_ASSERT(2 <= numSubExpr); // must be at least two

            // Read the first two sub-expressions.
            auto e1 = __readExpr(io_stream, i_isoChip, io_isoNode);
            auto e2 = __readExpr(io_stream, i_isoChip, io_isoNode);
            HEI_ASSERT(e1 && e2); // Cannot be null

            // Create the OR register and put it in the flyweights.
            expr = factory.get(e1, e2);

            // Iterate any remaining expressions.
            for (uint8_t i = 2; i < numSubExpr; i++)
            {
                // Read the next sub-expressions.
                e2 = __readExpr(io_stream, i_isoChip, io_isoNode);
                HEI_ASSERT(e2); // Cannot be null

                // Create the OR register and put it in the flyweights.
                expr = factory.get(expr, e2);
            }

            break;
        }
        case 0x12: // NOT operation
        {
            auto& factory = Flyweight<const NotRegister>::getSingleton();

            // Read the sub-expression
            auto e = __readExpr(io_stream, i_isoChip, io_isoNode);
            HEI_ASSERT(e); // Cannot be null

            // Create the NOT register and put it in the flyweights.
            expr = factory.get(e);

            break;
        }
        case 0x13: // left shift operation
        {
            auto& factory = Flyweight<const LeftShiftRegister>::getSingleton();

            uint8_t shiftValue;
            io_stream >> shiftValue;

            // Read the sub-expression
            auto e = __readExpr(io_stream, i_isoChip, io_isoNode);
            HEI_ASSERT(e); // Cannot be null

            // Create the left shift register and put it in the flyweights.
            expr = factory.get(e, shiftValue);

            break;
        }
        case 0x14: // right shift operation
        {
            auto& factory = Flyweight<const RightShiftRegister>::getSingleton();

            uint8_t shiftValue;
            io_stream >> shiftValue;

            // Read the sub-expression
            auto e = __readExpr(io_stream, i_isoChip, io_isoNode);
            HEI_ASSERT(e); // Cannot be null

            // Create the right shift register and put it in the flyweights.
            expr = factory.get(e, shiftValue);

            break;
        }
        default:
            HEI_ASSERT(false); // unsupported expression type
    }

    return expr;
}

//------------------------------------------------------------------------------

using TmpChildNodeMap = std::map<BitPosition_t, IsolationNode::Key>;
using TmpNodeData = std::pair<IsolationNode::Ptr, TmpChildNodeMap>;
using TmpNodeMap = std::map<IsolationNode::Key, TmpNodeData>;

void __readNode(ChipDataStream& io_stream, const IsolationChip::Ptr& i_isoChip,
                TmpNodeMap& io_tmpNodeMap, Version_t i_version)
{
    // Read the node metadata.
    NodeId_t nodeId;
    RegisterType_t regType;
    Instance_t numInsts;
    io_stream >> nodeId >> regType >> numInsts;

    // Version 3 and above: Read any write operations that exist.
    std::map<OpRuleName_t, std::pair<OpRuleType_t, RegisterId_t>> opRules;
    if (VERSION_3 <= i_version)
    {
        uint8_t numOpRules;
        io_stream >> numOpRules;
        for (unsigned int i = 0; i < numOpRules; i++)
        {
            OpRuleName_t opName;
            OpRuleType_t opType;
            RegisterId_t regId;
            io_stream >> opName >> opType >> regId;

            std::pair<OpRuleType_t, RegisterId_t> tmpPair = {opType, regId};
            auto ret = opRules.emplace(opName, tmpPair);
            HEI_ASSERT(ret.second || ret.first->second == tmpPair);
        }
    }

    for (unsigned int i = 0; i < numInsts; i++)
    {
        // Read the node instance metadata.
        Instance_t nodeInst;
        uint8_t numCapRegs, numIsoRules, numChildNodes;
        io_stream >> nodeInst >> numCapRegs >> numIsoRules >> numChildNodes;

        // It is possible to have rules defined and no child nodes, However, if
        // there are no rules defined (FFDC-only node), there should not be
        // any child nodes defined.
        HEI_ASSERT(0 != numIsoRules || 0 == numChildNodes);

        // Allocate memory for this isolation node.
        auto isoNode =
            std::make_shared<IsolationNode>(nodeId, nodeInst, regType);

        // Add capture registers.
        for (unsigned int j = 0; j < numCapRegs; j++)
        {
            // Read the capture register metadata.
            RegisterId_t regId;
            Instance_t regInst;
            io_stream >> regId >> regInst;

            BitPosition_t bit = MAX_BIT_POSITION; // default all bits
            if (VERSION_2 <= i_version)
            {
                io_stream >> bit;
            }

            // Find the hardware register that is stored in this isolation chip
            // and add it to the list of capture registers. Note that this will
            // assert that the target register must exist in the isolation chip.
            auto hwReg = i_isoChip->getHardwareRegister({regId, regInst});

            // Add the register to the isolation node.
            isoNode->addCaptureRegister(hwReg, bit);
        }

        // Add isolation rules.
        for (unsigned int j = 0; j < numIsoRules; j++)
        {
            // Read the rule metadata.
            AttentionType_t attnType;
            io_stream >> attnType;

            // Read out the rule for this attention type.
            auto rule = __readExpr(io_stream, i_isoChip, isoNode);
            HEI_ASSERT(rule); // Cannot be null

            // Add the rule to the isolation node.
            isoNode->addRule(attnType, rule);
        }

        // At this point, we will need to read out the child node metadata.
        // However, we can't look up the child nodes and add them to this
        // isolation node yet because we are still in the process of parsing
        // them out of the Chip Data File. Therefore, we'll save a temporary map
        // containing the child node information which will be used to look up
        // the actual node objects later.
        TmpChildNodeMap cMap{};
        for (unsigned int j = 0; j < numChildNodes; j++)
        {
            // Read the child node metadata.
            BitPosition_t bit;
            NodeId_t childId;
            Instance_t childInst;
            io_stream >> bit >> childId >> childInst;

            auto ret =
                cMap.emplace(bit, IsolationNode::Key{childId, childInst});
            HEI_ASSERT(ret.second); // Should not have duplicate entries
        }

        // Version 3 and above: Add any write operations to the isoNode.
        if (VERSION_3 <= i_version)
        {
            for (const auto& rule : opRules)
            {
                isoNode->addOpRule(rule.first, rule.second.first,
                                   rule.second.second);
            }
        }

        // Add this isolation node with the temporary child node map to the
        // returned map of nodes.
        auto ret = io_tmpNodeMap.emplace(IsolationNode::Key{nodeId, nodeInst},
                                         TmpNodeData{isoNode, cMap});
        HEI_ASSERT(ret.second); // Should not have duplicate entries
    }
}

//------------------------------------------------------------------------------

void __insertNodes(IsolationChip::Ptr& io_isoChip,
                   const TmpNodeMap& i_tmpNodeMap)
{
    for (const auto& n : i_tmpNodeMap)
    {
        const IsolationNode::Ptr& node = n.second.first;
        const TmpChildNodeMap& childMap = n.second.second;

        // Link the child nodes, if they exist.
        for (const auto& c : childMap)
        {
            const BitPosition_t& bit = c.first;
            const IsolationNode::Key& childKey = c.second;

            // Find the child node in the temporary map.
            auto itr = i_tmpNodeMap.find(childKey);
            HEI_ASSERT(i_tmpNodeMap.end() != itr); // Child node must exist.

            const IsolationNode::Ptr& child = itr->second.first;

            node->addChild(bit, child);
        }

        // Add this node to the isolation chip.
        io_isoChip->addIsolationNode(node);
    }
}

//------------------------------------------------------------------------------

void __readRoot(ChipDataStream& io_stream, IsolationChip::Ptr& io_isoChip)
{
    // Read the root node metadata.
    AttentionType_t attnType;
    NodeId_t id;
    Instance_t inst;
    io_stream >> attnType >> id >> inst;

    // Add the root node.
    io_isoChip->addRootNode(attnType, io_isoChip->getIsolationNode({id, inst}));
}

//------------------------------------------------------------------------------

void parseChipDataFile(void* i_buffer, size_t i_bufferSize,
                       IsolationChip::Map& io_isoChips)
{
    ChipDataStream stream{i_buffer, i_bufferSize};

    // Read the file metadata.
    FileKeyword_t fileKeyword;
    ChipType_t chipType;
    Version_t version;
    stream >> fileKeyword >> chipType >> version;

    // Check the file keyword.
    HEI_ASSERT(KW_CHIPDATA == fileKeyword);

    // This chip type should not already exist.
    HEI_ASSERT(io_isoChips.end() == io_isoChips.find(chipType));

    // Check supported versions.
    HEI_ASSERT(VERSION_1 <= version && version <= VERSION_3);

    // Allocate memory for the new isolation chip.
    auto isoChip = std::make_shared<IsolationChip>(chipType);

    // Read the register list metadata.
    SectionKeyword_t regsKeyword;
    RegisterId_t numRegs;
    stream >> regsKeyword >> numRegs;

    // Check the register keyword.
    HEI_ASSERT(KW_REGS == regsKeyword);

    // There must be at least one register defined.
    HEI_ASSERT(0 != numRegs);

    for (unsigned int i = 0; i < numRegs; i++)
    {
        __readRegister(stream, isoChip);
    }

    // Read the node list metadata.
    SectionKeyword_t nodeKeyword;
    NodeId_t numNodes;
    stream >> nodeKeyword >> numNodes;

    // Check the node keyword.
    HEI_ASSERT(KW_NODE == nodeKeyword);

    // There must be at least one node defined.
    HEI_ASSERT(0 != numNodes);

    TmpNodeMap tmpNodeMap; // Stores all nodes with child node map.
    for (unsigned int i = 0; i < numNodes; i++)
    {
        __readNode(stream, isoChip, tmpNodeMap, version);
    }
    // Link all nodes with their child nodes. Then add them to isoChip.
    __insertNodes(isoChip, tmpNodeMap);

    // Read the root node list metadata.
    SectionKeyword_t rootKeyword;
    AttentionType_t numRoots;
    stream >> rootKeyword >> numRoots;

    // Check the root node keyword.
    HEI_ASSERT(KW_ROOT == rootKeyword);

    // There must be at least one register defined.
    HEI_ASSERT(0 != numRoots);

    for (unsigned int i = 0; i < numRoots; i++)
    {
        __readRoot(stream, isoChip);
    }

    // At this point, the stream is done and it should be at the end of the
    // file.
    HEI_ASSERT(stream.eof());

    // Add this isolation chip to the collective list of isolation chips.
    auto ret = io_isoChips.emplace(chipType, isoChip);
    HEI_ASSERT(ret.second); // Just in case.
}

//------------------------------------------------------------------------------

} // namespace libhei
