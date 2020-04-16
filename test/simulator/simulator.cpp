#include "simulator.hpp"

#include <fstream> // std::ifstream

namespace libhei
{

//------------------------------------------------------------------------------

const std::map<SimulatorData::ChipType, const char*>
    SimulatorData::cv_chipPath = {
        {SAMPLE, "chip_data_sample.cdb"},
};

//------------------------------------------------------------------------------

void SimulatorData::addChip(const Chip& i_chip)
{
    // First check if this entry already exists.
    auto itr1 = std::find(iv_chipList.begin(), iv_chipList.end(), i_chip);
    ASSERT_EQ(iv_chipList.end(), itr1);

    // Add the new entry.
    iv_chipList.push_back(i_chip);

    // Look for the file path
    auto itr2 = cv_chipPath.find(static_cast<ChipType>(i_chip.getType()));
    ASSERT_NE(cv_chipPath.end(), itr2);
    const char* path = itr2->second;

    // Open the Chip Data File
    std::ifstream ifs{path, std::ifstream::binary};
    ASSERT_TRUE(ifs.good());

    ifs.close();
    /*
    if (ifs.is_open()) {
        // get length of file:
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);

        // allocate memory:
        char * buffer = new char [length];

        // read data as a block:
        is.read (buffer,length);

        is.close();

        // print content:
        std::cout.write (buffer,length);

        delete[] buffer;
      }
    */

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

} // end namespace libhei
