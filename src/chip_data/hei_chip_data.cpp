#include <chip_data/hei_chip_data.hpp>
#include <chip_data/hei_chip_data_stream.hpp>

namespace libhei
{

//------------------------------------------------------------------------------

using FileKeyword_t = uint64_t;

constexpr FileKeyword_t KW_CHIPDATA = 0x4348495044415441; // "CHIPDATA" ASCII

using SectionKeyword_t = uint32_t;

constexpr SectionKeyword_t KW_REGS = 0x52454753; // "REGS" ASCII
constexpr SectionKeyword_t KW_NODE = 0x4e4f4445; // "NODE" ASCII
constexpr SectionKeyword_t KW_ROOT = 0x524f4f54; // "ROOT" ASCII

using Version_t = uint8_t;

constexpr Version_t VERSION_1 = 0x01;

//------------------------------------------------------------------------------

void parseChipDataFile(void* i_buffer, size_t i_bufferSize,
                       IsolationChipMap& io_isoChips)
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

    // So far there is only one supported version type so check it here.
    HEI_ASSERT(VERSION_1 == version);

    // Allocate memory for the new isolation chip.
    auto isoChip = std::make_unique<IsolationChip>(chipType);

    // Read the register list metadata.
    SectionKeyword_t regsKeyword;
    RegisterId_t numRegs;
    stream >> regsKeyword >> numRegs;

    // Check the register keyword.
    HEI_ASSERT(KW_REGS == regsKeyword);

    // There must be at least one register defined.
    HEI_ASSERT(0 != numRegs);

    for (uint32_t i = 0; i < numRegs; i++)
    {
        // TODO
    }

    // Add this isolation chip to the collective list of isolation chips.
    auto ret = io_isoChips.emplace(chipType, std::move(isoChip));
    HEI_ASSERT(ret.second); // Just in case.
}

//------------------------------------------------------------------------------

} // namespace libhei
