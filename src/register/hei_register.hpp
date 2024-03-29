#pragma once

#include <util/hei_bit_string.hpp>
#include <util/hei_includes.hpp>

namespace libhei
{

/**
@brief An abstract class for register objects.

Purpose:  Register is an abstract base class for real and virtual registers.
A few examples of these registers are; HardwareRegister, ConstantRegister,
NotRegister, and AndRegister.  As a base class of register types, Register
creates a place where these registers can be put together like logical
building blocks.  For example, Register makes this possible:

Register * mask1 = new ConstantRegister(0xffff000000000000);
Register * mask2 = new ConstantRegister(0xffffffff00000000);
Register * fir_mask = new AndRegister(mask1, mask2);
const BitString * bs = fir_mask->getBitString(chip);

The getBitString function (defined by each register) provides access to
the BitString that manages each register's data.  In this example bs will
contain the result of mask1 & mask2.
*/
class Register
{
  public: // Aliases
    using Ptr = std::shared_ptr<Register>;
    using ConstPtr = std::shared_ptr<const Register>;

  public:
    /** @brief Pure virtual destructor. */
    virtual ~Register() = 0;

    /**
     * @brief  Provides access to the value of this register.
     * @param  i_chip Indicates which chip to access for this register.
     * @return A BitString containing the value of this register.
     */
    virtual const BitString* getBitString(const Chip& i_chip) const = 0;

    /** @return The size (in bytes) of this register. */
    virtual size_t getSize() const = 0;
};

// Pure virtual destructor must be defined.
inline Register::~Register() {}

} // end namespace libhei
