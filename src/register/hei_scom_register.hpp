#pragma once

#include <register/hei_hardware_register.hpp>
#include <util/hei_flyweight.hpp>

namespace libhei
{

/**
 * @brief A Power Systems SCOM register.
 *
 * Address width:   4 bytes
 * Register width:  8 bytes
 * Bit order:       Ascending (0-63 left to right)
 */
class ScomRegister : public HardwareRegister
{
  public: // Constructor, destructors, assignment, etc.
    /**
     * @brief Constructor from components.
     * @param i_chipType    Type of chip associated with this register.
     * @param i_id          Unique ID for this register.
     * @param i_instance    Instance of this register
     * @param i_accessLevel Hardware access level for this register.
     * @param i_address     A 4-byte address for this SCOM register.
     */
    ScomRegister(ChipType_t i_chipType, RegisterId_t i_id,
                 Instance_t i_instance, RegisterAccessLevel_t i_accessLevel,
                 uint32_t i_address) :
        HardwareRegister(i_chipType, i_id, i_instance, i_accessLevel),
        iv_address(i_address)
    {}

    /** @brief Destructor. */
    ~ScomRegister() = default;

  private:
    // This is needed to allow the flyweights to use the copy constructor, but
    // not allow it to be used in general.
    friend class Flyweight<ScomRegister>;

    /**
     * @brief Copy constructor.
     *
     * Needed by Flyweight class, but should not be allowed in general.
     */
    ScomRegister(const ScomRegister&) = default;

    /**
     * @brief Explicitly disables assignment operator.
     *
     * This is redundant since the compilier will implicitly delete this because
     * of the constant instance variables, but helps communicate it is not
     * allowed.
     */
    ScomRegister& operator=(const ScomRegister&) = delete;

  public: // Accessor functions
    /** Function overloaded from parent HardwareRegister class. */
    RegisterType_t getRegisterType() const
    {
        return REG_TYPE_SCOM;
    }

    /** Function overloaded from parent HardwareRegister class. */
    RegisterAddress_t getAddress() const
    {
        return static_cast<RegisterAddress_t>(iv_address);
    }

    /** Function overloaded from parent HardwareRegister class. */
    size_t getSize() const
    {
        return 8;
    }

  private: // Instance variables
    /** This register's address. */
    const uint32_t iv_address;

}; // end class ScomRegister

/**
 * @brief A Power Systems Indirect SCOM register.
 *
 * Address width:   8 bytes
 * Register width:  2* bytes (see note below)
 * Bit order:       Ascending (0-63 left to right)
 *
 * IMPORTANT NOTE:
 *   Technically, only two bytes of data are actually used. However, the bit
 *   definition of these registers put the two bytes at the end of the returned
 *   value (bit 48-63). Therefore, this class will be made to look like the
 *   width is 8 bytes in order to make the bit indexing work in the returned
 *   BitString.
 */
class IdScomRegister : public HardwareRegister
{
  public: // Constructor, destructors, assignment, etc.
    /**
     * @brief Constructor from components.
     * @param i_chipType    Type of chip associated with this register.
     * @param i_id          Unique ID for this register.
     * @param i_instance    Instance of this register
     * @param i_accessLevel Hardware access level for this register.
     * @param i_address     An 8-byte address for this Indirect SCOM register.
     */
    IdScomRegister(ChipType_t i_chipType, RegisterId_t i_id,
                   Instance_t i_instance, RegisterAccessLevel_t i_accessLevel,
                   uint64_t i_address) :
        HardwareRegister(i_chipType, i_id, i_instance, i_accessLevel),
        iv_address(i_address)
    {}

    /** @brief Destructor. */
    ~IdScomRegister() = default;

  private:
    // This is needed to allow the flyweights to use the copy constructor, but
    // not allow it to be used in general.
    friend class Flyweight<IdScomRegister>;

    /**
     * @brief Copy constructor.
     *
     * Needed by Flyweight class, but should not be allowed in general.
     */
    IdScomRegister(const IdScomRegister&) = default;

    /**
     * @brief Explicitly disables assignment operator.
     *
     * This is redundant since the compilier will implicitly delete this because
     * of the constant instance variables, but helps communicate it is not
     * allowed.
     */
    IdScomRegister& operator=(const IdScomRegister&) = delete;

  public: // Accessor functions
    /** Function overloaded from parent HardwareRegister class. */
    RegisterType_t getRegisterType() const
    {
        return REG_TYPE_ID_SCOM;
    }

    /** Function overloaded from parent HardwareRegister class. */
    RegisterAddress_t getAddress() const
    {
        return static_cast<RegisterAddress_t>(iv_address);
    }

    /** Function overloaded from parent HardwareRegister class. */
    size_t getSize() const
    {
        return 8; // See note in class documentation.
    }

  private: // Instance variables
    /** This register's address. */
    const uint64_t iv_address;

}; // end class IdScomRegister

} // end namespace libhei
