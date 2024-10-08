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
     * @param i_id       Unique ID for this register.
     * @param i_instance Instance of this register
     * @param i_flags    Attribute flags for this register.
     * @param i_address  A 4-byte address for this SCOM register.
     */
    ScomRegister(RegisterId_t i_id, Instance_t i_instance,
                 RegisterAttributeFlags_t i_flags, uint32_t i_address) :
        HardwareRegister(i_id, i_instance, i_flags), iv_address(i_address)
    {}

    /** @brief Destructor. */
    ~ScomRegister() = default;

  private:
    /** @brief Copy constructor. */
    ScomRegister(const ScomRegister&) = delete;

    /** @brief Assignment operator. */
    ScomRegister& operator=(const ScomRegister&) = delete;

  public: // Accessor functions
    /** Function overloaded from parent HardwareRegister class. */
    RegisterType_t getType() const override
    {
        return REG_TYPE_SCOM;
    }

    /** Function overloaded from parent HardwareRegister class. */
    RegisterAddress_t getAddress() const override
    {
        return static_cast<RegisterAddress_t>(iv_address);
    }

    /** Function overloaded from parent HardwareRegister class. */
    size_t getSize() const override
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
     * @param i_id       Unique ID for this register.
     * @param i_instance Instance of this register
     * @param i_flags    Attribute flags for this register.
     * @param i_address  An 8-byte address for this Indirect SCOM register.
     */
    IdScomRegister(RegisterId_t i_id, Instance_t i_instance,
                   RegisterAttributeFlags_t i_flags, uint64_t i_address) :
        HardwareRegister(i_id, i_instance, i_flags), iv_address(i_address)
    {}

    /** @brief Destructor. */
    ~IdScomRegister() = default;

  private:
    /** @brief Copy constructor. */
    IdScomRegister(const IdScomRegister&) = delete;

    /** @brief Assignment operator. */
    IdScomRegister& operator=(const IdScomRegister&) = delete;

  public: // Accessor functions
    /** Function overloaded from parent HardwareRegister class. */
    RegisterType_t getType() const override
    {
        return REG_TYPE_ID_SCOM;
    }

    /** Function overloaded from parent HardwareRegister class. */
    RegisterAddress_t getAddress() const override
    {
        return static_cast<RegisterAddress_t>(iv_address);
    }

    /** Function overloaded from parent HardwareRegister class. */
    size_t getSize() const override
    {
        return 8; // See note in class documentation.
    }

  private: // Instance variables
    /** This register's address. */
    const uint64_t iv_address;

}; // end class IdScomRegister

} // end namespace libhei
