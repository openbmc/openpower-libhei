#pragma once

/**
@file hei_register.hpp
@brief Description:  The Register class.

Class Specification

Name:     Register
Title:    Scan Communication Register
Purpose:  "Register" provides representation and access to a
          physical register.
Notes:    The Register class is a model of an actual physical register.
          The bits in the register are represented by the bit_string
          data member which is modified dynamically as operations are
          performed.  It acts as a temporarily cached value of the
          register.

          The current value of this cached bit string can be accessed
          or modified by other objects via the public interface.  The
          physical address and bit length of the hardware register are
          set during initialization and used on all accesses.
*/

#include <stdio.h>
#include <hei_includes.hpp>
#include <util/hei_bit_string.hpp>

namespace libhei
{

/**
Register class
Register provides representation and access to a physical register.
*/
class Register
{
  public:

    /**
    @brief Default constructor
    */
    Register() = default;

    /**
    @brief Delete copy, assignment, and move operators.  Deleting
           copy causes the compiler to delete move.
    */
    Register(const Register&) = delete;
    Register& operator=(const Register&) = delete;

    /**
    @brief Default destructor
    */
    virtual ~Register() = default;

    /**
    @brief      The BitString class is a utility class for managing a virtual
                register (like this one).  The getBitString function here
                provides access to this class. Note that this is a pure virtual
                function that must be implemented to use this class.
    @return     A pointer to a BitString.
    */
    virtual const BitString * getBitString() const = 0;

};

}//end namespace libhei
