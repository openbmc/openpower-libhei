#pragma once

#include <hei_signature.hpp>
#include <util/hei_bit_string.hpp>

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

namespace libhei
{

/**
 * @brief The main isolate() API is given a list of chips to analyze. This class
 *        will contain a list of all active hardware errors found on those
 *        chips, the contents of any registers associated with the active
 *        errors, and any other data that can be useful for debug.
 */
class IsolationData
{
  public: // Constructors, destructor, assignment, etc.
    /** @brief Default constructor. */
    IsolationData() = default;

    /** @brief Destructor. */
    ~IsolationData() = default;

    /** @brief Copy constructor. */
    IsolationData(const IsolationData&) = default;

    /** @brief Assignment operator. */
    IsolationData& operator=(const IsolationData&) = default;

  public:
    /** The data stored in each entry of the register dump. */
    struct RegDumpEntry
    {
        RegDumpEntry(RegisterId_t i_regId, Instance_t i_regInst,
                     std::shared_ptr<BitStringBuffer> i_data) :
            regId(i_regId),
            regInst(i_regInst), data(i_data)
        {}

        RegisterId_t regId;                    ///< 3-byte register ID
        Instance_t regInst;                    ///< 1-byte register instance
        std::shared_ptr<BitStringBuffer> data; ///< register data

        bool operator==(const RegDumpEntry& r) const
        {
            return regId == r.regId && regInst == r.regInst && *data == *r.data;
        }
    };

  private: // Instance variables
    /** A list of all signatures found during isolation. */
    std::vector<Signature> iv_sigList;

    /**
     * This intended to be a snapshot of the register values read from hardware
     * as the isolator iterates the isolation tree. Therefore, it cannot share
     * the values stored in the hardware register cache. Instead, it must be a
     * copy of the data.
     */
    std::map<Chip, std::vector<RegDumpEntry>> iv_regDump;

  public: // Member functions
    /**
     * @brief Adds a signature to the signature list.
     * @param i_signature The target signature.
     */
    void addSignature(const Signature& i_signature)
    {
        iv_sigList.push_back(i_signature);
    }

    /**
     * @brief Adds the contents of a register to the register dump.
     * @param i_chip    The chip associated with this register.
     * @param i_regId   The register ID.
     * @param i_regInst The register instance.
     * @param i_data    A BitString containing the contents of the register.
     *                  Note that this function will make a copy of the data,
     *                  which will be stored separately from the hardware
     *                  register cache.
     */
    void addRegister(const Chip& i_chip, RegisterId_t i_regId,
                     Instance_t i_regInst, const BitString* i_data)
    {
        if (!i_data->isZero()) // Only add non-zero values to save space.
        {
            // Make a copy of the register value.
            auto data = std::make_shared<BitStringBuffer>(*i_data);

            // We'll want to avoid adding duplicate data if possible.
            if (iv_regDump.end() == iv_regDump.find(i_chip))
            {
                // The chip doesn't exist in the map. Therefore, the data
                // doesn't currently exist. So, add it.
                iv_regDump[i_chip].emplace_back(i_regId, i_regInst, data);
            }
            else
            {
                // Search this chip for the data.
                RegDumpEntry entry{i_regId, i_regInst, data};
                auto itr = std::find(iv_regDump[i_chip].begin(),
                                     iv_regDump[i_chip].end(), entry);

                if (iv_regDump[i_chip].end() == itr)
                {
                    // The data doesn't currently exist. So, add it.
                    iv_regDump[i_chip].push_back(entry);
                }
            }
        }
    }

    /** @brief Allows access to the signature list. */
    const std::vector<Signature>& getSignatureList() const
    {
        return iv_sigList;
    }

    /** @brief Allows access to the register dump. */
    const std::map<Chip, std::vector<RegDumpEntry>>& getRegisterDump() const
    {
        return iv_regDump;
    }

    /** @brief Flushes the data to ensure a clean slate for isolation. */
    void flush()
    {
        iv_sigList.clear();
        iv_regDump.clear();
    }

    /** @brief Queries the signature list for any checkstop attentions. */
    bool queryCheckstop()
    {
        auto itr = std::find_if(
            iv_sigList.begin(), iv_sigList.end(), [](const auto& s) {
                return ATTN_TYPE_CHECKSTOP == s.getAttnType();
            });

        return iv_sigList.end() != itr;
    }

}; // end class IsolationData

} // end namespace libhei
