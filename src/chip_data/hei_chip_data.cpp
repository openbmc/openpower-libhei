#include <chip_data/hei_chip_data.hpp>
#include <chip_data/hei_chip_data_stream.hpp>

namespace libhei
{

//------------------------------------------------------------------------------

enum Keyword
{
    KW_CHIPDATA = 0x4348495044415441, // ASCII values for "CHIPDATA"
    KW_REGS     = 0x43484950,         // ASCII values for "REGS"
    KW_NODE     = 0x4e4f4445,         // ASCII values for "NODE"
    KW_ROOT     = 0x524f4f54,         // ASCII values for "ROOT"
};

// Supported file versions.
enum Version
{
    VERSION_1 = 0x01,
};

//------------------------------------------------------------------------------

void parseChipDataFile(void* i_buffer, size_t i_bufferSize,
                       IsolationChipMap& io_isoChips)
{
    ChipDataStream stream{i_buffer, i_bufferSize};

    // Read the file metadata.
    uint64_t keyword    = 0;
    ChipType_t chipType = 0;
    uint8_t version     = 0;
    stream >> keyword >> chipType >> version;

    // Check the file ID.
    HEI_ASSERT(KW_CHIPDATA == keyword);

    // This chip type should not already exist.
    HEI_ASSERT(io_isoChips.end() == io_isoChips.find(chipType));

    // Allocate memory for the new isolation chip.
    IsolationChipPtr isoChip{new IsolationChip{chipType}};

    // TODO

    // Add this isolation chip to the collective list of isolation chips.
    auto ret = io_isoChips.emplace(chipType, isoChip);
    HEI_ASSERT(ret.second); // Just in case.
}

//------------------------------------------------------------------------------

} // namespace libhei
