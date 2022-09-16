#pragma once

#include <register/hei_register.hpp>
#include <util/hei_bit_string.hpp>
#include <util/hei_includes.hpp>

namespace libhei
{

/**
 * @brief An abstract class containing information (e.g. address, type, length,
 *        etc.) for an actual hardware register.
 *
 * Hardware access:
 *
 *  Actual hardware access is defined by the user application via the user
 *  interface APIs. In order to tell the user application which chip to target,
 *  the user application will give the isolator a list of pointers to its
 *  objects. They will then be passed into the public functions of this class
 *  and eventually given back to the user application when hardware access is
 *  needed.
 *
 * Register cache:
 *
 *  In order to save memory space, each instance of this class does not store
 *  the contents of the target hardware register. Instead, that data is stored
 *  in a register cache, which is a static variable defined in this class. This
 *  allows us to store only what we need. The cache can also be thought of as a
 *  snapshot of the registers at the time of isolation, which can be useful if
 *  the hardware is still running and register values could change.
 *
 *  In order to ensure stale data isn't used from the cache, call
 *  HardwareRegister::flushAll() before beginning isolation on a new attention.
 *  Also, HardwareRegister::flushAll() should be called when the isolator is
 *  uninitialized before the rest of the isolation objects are deleted.
 */
class HardwareRegister : public Register
{
  public: // Aliases
    using Ptr      = std::shared_ptr<HardwareRegister>;
    using ConstPtr = std::shared_ptr<const HardwareRegister>;

    using Key = std::pair<RegisterId_t, Instance_t>;

  public:
    /** @brief Pure virtual destructor. */
    virtual ~HardwareRegister() = 0;

  protected:
    /**
     * @brief Constructor from components.
     * @param i_id       Unique ID for this register.
     * @param i_instance Instance of this register
     * @param i_flags    Attribute flags for this register.
     */
    HardwareRegister(RegisterId_t i_id, Instance_t i_instance,
                     RegisterAttributeFlags_t i_flags) :
        Register(),
        iv_id(i_id), iv_instance(i_instance), iv_flags(i_flags)
    {}

  private: // Instance variables
    /** The unique ID for this register. */
    const RegisterId_t iv_id;

    /** A register may have multiple instances. All of which will have the same
     *  ID. This variable is used to distinguish between each instance of the
     *  register. */
    const Instance_t iv_instance;

    /** The hardware access level of this register (read/write, read-only,
     *  write-only, etc.). */
    const RegisterAttributeFlags_t iv_flags;

  public: // Accessor functions
    /* @return The unique ID for this register. */
    RegisterId_t getId() const
    {
        return iv_id;
    }

    /* @return The instance of this register. */
    Instance_t getInstance() const
    {
        return iv_instance;
    }

    /** @return The register/instance key. */
    Key getKey() const
    {
        return {iv_id, iv_instance};
    }

    /** @return True if given flag is enabled, false if disabled. */
    bool queryAttrFlag(RegisterAttributeFlags_t i_flag) const
    {
        return (0 != (iv_flags & i_flag));
    }

    // NOTE: The following are determined by child classes.

    /** @return This register's type. */
    virtual RegisterType_t getType() const = 0;

    /** @return The address of this register. */
    virtual RegisterAddress_t getAddress() const = 0;

  public: // Operators
    /** @brief Equals operator. */
    bool operator==(const HardwareRegister& i_r) const
    {
        // In general, comparing the ID and instance should be enough. However,
        // no error will be thrown when adding the register to the flyweights
        // and any other field differs. Therefore, all fields will be used and
        // invalid duplicates will be found when adding the register pointers
        // to the IsolationChip objects.
        return (getAddress() == i_r.getAddress()) && (getId() == i_r.getId()) &&
               (getInstance() == i_r.getInstance()) &&
               (getType() == i_r.getType()) && (iv_flags == i_r.iv_flags);
    }

    /** @brief Less than operator. */
    bool operator<(const HardwareRegister& i_r) const
    {
        // In general, comparing the ID and instance should be enough. However,
        // no error will be thrown when adding the register to the flyweights
        // and any other field differs. Therefore, all fields will be used and
        // invalid duplicates will be found when adding the register pointers
        // to the IsolationChip objects.
        if (getAddress() < i_r.getAddress())
        {
            return true;
        }
        else if (getAddress() == i_r.getAddress())
        {
            if (getId() < i_r.getId())
            {
                return true;
            }
            else if (getId() == i_r.getId())
            {
                if (getInstance() < i_r.getInstance())
                {
                    return true;
                }
                else if (getInstance() == i_r.getInstance())
                {
                    if (getType() < i_r.getType())
                    {
                        return true;
                    }
                    else if (getType() == i_r.getType())
                    {
                        return (iv_flags < i_r.iv_flags);
                    }
                }
            }
        }

        return false;
    }

  public:
    /** Function overloaded from parent Register class. */
    const BitString* getBitString(const Chip& i_chip) const override;

    /**
     * @brief  Reads a register from hardware via the user interface APIs.
     * @param  i_chip  The target chip in which this register belongs.
     * @param  i_force When false, this function will only read from hardware if
     *                 an entry for this instance does not already exist in the
     *                 register cache. When true, the entry in the register
     *                 cache is flushed, if it exists. Then this function will
     *                 read from hardware and update the cache.
     * @return See the return code from the registerRead() user interface API.
     */
    bool read(const Chip& i_chip, bool i_force = false) const;

#ifdef __HEI_ENABLE_HW_WRITE

    /**
     * @brief  Writes the value stored in the register cache to hardware via the
     *         user interface APIs.
     * @param  i_chip  The target chip in which this register belongs.
     * @return See the return code from the registerWrite() user interface API.
     */
    bool write(const Chip& i_chip) const;

#endif // __HEI_ENABLE_HW_WRITE

  protected:
    /**
     * @brief  Provides access to this register's BitString.
     *
     * WARNING: Allowing public access to this function may be dangerous. For
     *          now it should be left as protected.
     *
     * @param  i_chip  The target chip in which this register belongs.
     * @return A reference to the BitString.
     */
    BitString& accessBitString(const Chip& i_chip);

  private: // Register cache class variable
    /**
     * @brief Caches the contents of registers read from hardware.
     *
     * The goal is to create a snapshot of the hardware register contents as
     * close to the reported attention as possible. This snapshot is then used
     * for additional analysis/debug when needed.
     */
    class Cache
    {
      public:
        /** @brief Default constructor. */
        Cache() = default;

        /** @brief Destructor. */
        ~Cache() = default;

        /** @brief Copy constructor. */
        Cache(const Cache&) = delete;

        /** @brief Assignment operator. */
        Cache& operator=(const Cache&) = delete;

        /**
         * @brief  Queries if a specific entry exists in the cache.
         * @param  i_chip  The target chip.
         * @param  i_hwReg The target register.
         * @return True if the entry exists, false otherwise.
         */
        bool query(const Chip& i_chip, const HardwareRegister* i_hwReg) const;

        /**
         * @brief  Returns the data buffer for the given chip and register.
         * @param  i_chip  The target chip.
         * @param  i_hwReg The target register.
         * @return A reference to the BitString containing the register data.
         * @note   If an entry does not exist in the cache, an entry will be
         *         created and the BitString will be initialized to 0.
         */
        BitString& access(const Chip& i_chip, const HardwareRegister* i_hwReg);

        /** @brief Flushes entire contents from cache. */
        void flush();

        /**
         * @brief Removes a single register from the cache.
         * @param i_chip  The target chip.
         * @param i_hwReg The target register.
         */
        void flush(const Chip& i_chip, const HardwareRegister* i_hwReg);

      private:
        /**
         * @brief Stores a BitStringBuffer for each HardwareRegister per Chip.
         *
         * The HardwareRegister keys will just be pointers to the isolation
         * objects created in the main initialize() API. Those should exist
         * until the main uninitialize() API is called. It is important that the
         * cache is flushed at the beginning of the uninitialize() API before
         * the rest of the isolation objects are deleted.
         *
         * The Chip keys are copies of the objects passed to the isolator
         * because the user application is responsible for storage of the
         * objects passed to the isolator. We don't want to chance a Chip was
         * created as a local variable that goes out of scope, or other similar
         * situations.
         */
        std::map<Chip, std::map<const HardwareRegister*, BitString*>> iv_cache;
    };

    /** This allows all HardwareRegister objects access to the cache. */
    static Cache cv_cache;

  public: // Register cache management functions.
    /** @brief Flushes the entire register cache. */
    static void flushAll()
    {
        cv_cache.flush();
    }

    /**
     * @brief Flushes this register from the cache.
     * @param  i_chip  The target chip in which this register belongs.
     */
    void flush(const Chip& i_chip) const
    {
        cv_cache.flush(i_chip, this);
    }

  private: // Register cache management functions.
    /**
     * @param  i_chip  The target chip in which this register belongs.
     * @return True if an entry for this register exist in this cache.
     */
    bool queryCache(const Chip& i_chip) const
    {
        return cv_cache.query(i_chip, this);
    }

    /**
     * @param  i_chip  The target chip in which this register belongs.
     * @return A reference to this register's BitString in cache.
     */
    BitString& accessCache(const Chip& i_chip) const
    {
        return cv_cache.access(i_chip, this);
    }
};

} // end namespace libhei
