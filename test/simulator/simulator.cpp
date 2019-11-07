#include "simulator.hpp"

#include <hei_user_interface.hpp>

namespace libhei
{

// This is the simulated implementation of the user defined API.
ReturnCode registerRead(const Chip& i_chip, void* o_buffer, size_t& io_bufSize,
                        uint64_t i_regType, uint64_t i_address)
{
    ReturnCode rc{};

    HEI_ASSERT(nullptr != o_buffer);
    HEI_ASSERT(0 != io_bufSize);

    switch (i_regType)
    {
        // TODO: add cases for REG_TYPE_SCOM and REG_TYPE_ID_SCOM
        default:
            rc = RC_REG_ACCESS_FAILURE;
            HEI_ERR("registerRead(%p,%p,%lx,%lx,%lx)", i_chip.getChip(),
                    o_buffer, io_bufSize, i_regType, i_address);
    }

    return rc;
}

//------------------------------------------------------------------------------

#ifndef __HEI_READ_ONLY

ReturnCode registerWrite(const Chip& i_chip, void* i_buffer, size_t& io_bufSize,
                         uint64_t i_regType, uint64_t i_address)
{
    ReturnCode rc{};

    HEI_ASSERT(nullptr != i_buffer);
    HEI_ASSERT(0 != io_bufSize);

    switch (i_regType)
    {
        // TODO: add cases for REG_TYPE_SCOM and REG_TYPE_ID_SCOM
        default:
            rc = RC_REG_ACCESS_FAILURE;
            HEI_ERR("registerWrite(%p,%p,%lx,%lx,%lx)", i_chip.getChip(),
                    i_buffer, io_bufSize, i_regType, i_address);
    }

    return rc;
}

#endif

//------------------------------------------------------------------------------

void SimulatorData::addChip(const Chip& i_chip)
{
    // First check if this entry already exists.
    auto itr = std::find(iv_chipList.begin(), iv_chipList.end(), i_chip);
    ASSERT_EQ(iv_chipList.end(), itr);

    // Add the new entry.
    iv_chipList.push_back(i_chip);

    // TODO: Find the chip data file based on the chip type from i_chip. Read
    //       that file in to memory and call initialize.
    void* buffer     = nullptr;
    size_t sz_buffer = 0;

    ReturnCode rc = initialize(buffer, sz_buffer);
    ASSERT_TRUE(RC_SUCCESS == rc || RC_CHIP_DATA_INITIALIZED == rc);
}

//------------------------------------------------------------------------------

void SimulatorData::endIteration()
{
    // Start by calling libhei::isolate().
    IsolationData isoData{};
    ReturnCode rc = isolate(iv_chipList, isoData);

    // It is possible that even in a failure scenario the information in the
    // returned IsolationData would be useful.
    // TODO: Figure out where to put the data.

    // Verify if isolation completed successfully.
    ASSERT_TRUE(RC_SUCCESS == rc);

    // Get the list of signatures found in isolation.
    std::vector<Signature> givenSigList = isoData.getSignatureList();

    // Verify the expected list and given list are the same.
    ASSERT_EQ(iv_expSigList.size(), givenSigList.size());

    std::sort(iv_expSigList.begin(), iv_expSigList.end());
    std::sort(givenSigList.begin(), givenSigList.end());

    ASSERT_TRUE(std::equal(givenSigList.begin(), givenSigList.end(),
                           iv_expSigList.begin()));

    // The iteration is complete so we can flush the data.
    flushIterationData();
}

//------------------------------------------------------------------------------

} // end namespace libhei
