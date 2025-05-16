#pragma once

#include <chip_data/hei_chip_data.hpp>
#include <hei_isolation_data.hpp>
#include <isolator/hei_isolation_node.hpp>

namespace libhei
{

/**
 * @brief This class is a complement to the main APIs. Its purpose is to store
 *        and maintain all of the objects necessary for isolation.
 *
 * The intended flow is to:
 *  - Create a singleton instance of an Isolator object via getSingleton().
 *  - Use initialize() to input each necessary Chip Data File provided by the
 *    user application.
 *  - Call isolate() to find all active errors being reported by the given list
 *    of chips.
 *  - Once isolation is no longer needed, use uninitialize() to free up
 *    internal resources.
 *
 * The purpose of the singleton instance is to avoid initializing the object
 * each time isolation is required. The data provided by the Chip Data Files is
 * static. So reinitializing would be a waste of time, unless for some reason
 * the Chip Data Files themselves are updated, which would require
 * reinitialization anyway. Of course, leaving the object in memory chews up
 * resources. So, some users may need to weigh performance vs. memory usage.
 */
class Isolator
{
  private: // This class cannot be instantiated. Use getSingleton() instead.
    /** @brief Default constructor. */
    Isolator() = default;

    /** @brief Destructor. */
    ~Isolator()
    {
        // Clear out all of the internal isolation objects.
        uninitialize();
    }

    /** @brief Copy constructor. */
    Isolator(const Isolator&) = delete;

    /** @brief Assignment operator. */
    Isolator& operator=(const Isolator&) = delete;

  private:
    /** Keeps track of all chip types that have been initialized. */
    IsolationChip::Map iv_isoChips;

  public:
    /** @brief Provides access to a singleton instance of this object. */
    static Isolator& getSingleton()
    {
        static Isolator theIsolator;
        return theIsolator;
    }

    /**
     * @brief Provides access to an isolation chip within this object.
     * @param i_chipType The chip type of the isolation chip to access.
     * @return ConstPtr to the isolation chip object.
     */
    IsolationChip::ConstPtr getIsoChip(ChipType_t i_chipType) const
    {
        auto itr = iv_isoChips.find(i_chipType);
        HEI_ASSERT(iv_isoChips.end() != itr); // The IsolationChip should exist.

        return itr->second;
    }

    /** @brief See API wrapper description in hei_main.hpp. */
    void initialize(void* i_buffer, size_t i_bufferSize)
    {
        parseChipDataFile(i_buffer, i_bufferSize, iv_isoChips);
    }

    /**
     * @brief See API wrapper description in hei_main.hpp.
     *
     * This function is called in the destructor. Therefore, it should never
     * throw an exception.
     */
    void uninitialize();

    /** @brief See API wrapper description in hei_main.hpp. */
    void isolate(const std::vector<Chip>& i_chipList,
                 IsolationData& o_isoData) const;

#ifdef __HEI_ENABLE_HW_WRITE
    /**
     * @brief  Performs the given write operation on the input signature. The
     *         node and bit in the input signature will determine which register
     *         and bit are to be written by the operation.
     * @param  i_op  The given write operation rule. See the OpRuleName_t enum
     *               for supported values.
     * @param  i_sig The signature to determine what's to be written.
     * @return True if write operation failed, false if successful.
     */
    bool performWriteOp(OpRuleName_t i_op, Signature i_sig);
#endif

}; // end class Isolator

} // end namespace libhei
