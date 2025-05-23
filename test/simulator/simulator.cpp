#include "simulator.hpp"

#include <util/hei_includes.hpp>

#include <fstream> // std::ifstream

namespace libhei
{

//------------------------------------------------------------------------------

// Paths are relative from the build/ directory
const std::map<SimulatorData::SimChipType, const char*>
    SimulatorData::cv_chipPath = {
        {SAMPLE, "../test/simulator/sample_data/sample.cdb"},
        {EXPLORER_11, "chip_data/chip_data_explorer_11.cdb"},
        {EXPLORER_20, "chip_data/chip_data_explorer_20.cdb"},
        {ODYSSEY_10, "chip_data/chip_data_odyssey_10.cdb"},
        {P10_10, "chip_data/chip_data_p10_10.cdb"},
        {P10_20, "chip_data/chip_data_p10_20.cdb"},
};

//------------------------------------------------------------------------------

void SimulatorData::addChip(const Chip& i_chip, const char* i_chipPath)
{
    // First check if this entry already exists.
    auto chip_itr = std::find(iv_chipList.begin(), iv_chipList.end(), i_chip);
    ASSERT_EQ(iv_chipList.end(), chip_itr);

    // Add the new entry.
    iv_chipList.push_back(i_chip);

    // Check if this chip type has been initialized.
    ChipType_t chipType = i_chip.getType();
    auto type_itr = std::find(iv_typeList.begin(), iv_typeList.end(), chipType);
    if (iv_typeList.end() != type_itr)
    {
        return; // No need to continue.
    }

    // Add the new entry.
    iv_typeList.push_back(chipType);

    // Look for the file path, if a path was provided use that instead of one
    // of the hardcoded values in the map.
    ASSERT_TRUE(nullptr != i_chipPath);
    const char* path = "";
    if (0 != strcmp(i_chipPath, ""))
    {
        path = i_chipPath;
    }
    else
    {
        auto itr2 = cv_chipPath.find(static_cast<SimChipType>(chipType));
        ASSERT_NE(cv_chipPath.end(), itr2);
        path = itr2->second;
    }

    // Open the Chip Data File
    std::ifstream cdf{path, std::ifstream::binary};
    ASSERT_TRUE(cdf.good());

    // Get the length of file
    cdf.seekg(0, cdf.end);
    size_t sz_buffer = cdf.tellg();
    cdf.seekg(0, cdf.beg);

    // Allocate memory
    char* buffer = new char[sz_buffer];

    // Read data as a block
    cdf.read(buffer, sz_buffer);

    // Close the Chip Data File
    cdf.close();

    // Initilize the chip with this Chip Data File.
    initialize(buffer, sz_buffer);

    // Clean up the buffer
    delete[] buffer;
}

//------------------------------------------------------------------------------

const char* __attn(AttentionType_t i_type)
{
    const char* str = "";
    switch (i_type)
    {
        case ATTN_TYPE_CHIP_CS:
            str = "CHIP_CS";
            break;
        case ATTN_TYPE_UNIT_CS:
            str = "UNIT_CS";
            break;
        case ATTN_TYPE_RECOVERABLE:
            str = "RECOVERABLE";
            break;
        case ATTN_TYPE_SP_ATTN:
            str = "SP_ATTN";
            break;
        case ATTN_TYPE_HOST_ATTN:
            str = "HOST_ATTN";
            break;
        default:
            HEI_ERR("Unsupported attention type: %u", i_type);
            assert(0);
    }
    return str;
}

//------------------------------------------------------------------------------

void SimulatorData::simIsolate(IsolationData& o_isoData)
{
    // Start by calling libhei::isolate().
    IsolationData isoData{};
    isolate(iv_chipList, o_isoData);

    /* TODO: Currently used for debug. Eventually, we want this written to file.
    for (const auto& e : isoData.getRegisterDump())
    {
        HEI_INF("Chip: %s", (const char*)e.first.getChip());

        for (const auto& r : e.second)
        {
            HEI_INF("  Reg: 0x%06x  %d  0x%016" PRIx64, r.regId, r.regInst,
                    r.data->getFieldRight(0, 64));
        }
    }
    */

    // Get the list of signatures found in isolation.
    std::vector<Signature> givenSigList = o_isoData.getSignatureList();

    // Print out the expected signature list and verify matches with given list.
    HEI_INF("Signature summary:")
    bool mismatch = false;
    for (const auto& e : iv_expSigList)
    {
        auto gItr = std::find(givenSigList.begin(), givenSigList.end(), e);
        if (givenSigList.end() != gItr)
        {
            HEI_INF("  Match:      %s 0x%04x %2u %2u %s",
                    (const char*)e.getChip().getChip(), e.getId(),
                    e.getInstance(), e.getBit(), __attn(e.getAttnType()));

            // Remove from given signature list so we can determine if there are
            // any leftovers at the end.
            givenSigList.erase(gItr);
        }
        else
        {
            HEI_INF("  No match:   %s 0x%04x %2u %2u %s",
                    (const char*)e.getChip().getChip(), e.getId(),
                    e.getInstance(), e.getBit(), __attn(e.getAttnType()));

            mismatch = true;
        }
    }

    // Print out any leftover signatures from the given list.
    for (const auto& g : givenSigList)
    {
        HEI_INF("  Unexpected: %s 0x%04x %2u %2u %s",
                (const char*)g.getChip().getChip(), g.getId(), g.getInstance(),
                g.getBit(), __attn(g.getAttnType()));

        mismatch = true;
    }

    // Final check for mismatches.
    ASSERT_FALSE(mismatch);
}

//------------------------------------------------------------------------------

void SimulatorData::endIteration()
{
    // First call simIsolate to isolate and check the expected signatures.
    IsolationData isoData{};
    simIsolate(isoData);

    // The iteration is complete so we can flush the data.
    flushIterationData();
}

} // end namespace libhei
