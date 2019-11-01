#pragma once

#include "stdio.h"

/**
 * @file hei_operator_register.hpp
 *
 *
 * @brief A library of useful classes used to perform logical or bitwise
 *        math on or between other registers.  The classes implimented here
 *        include NOTRegister, LeftShiftRegister, RightShiftRegister,
 *        AndRegister, OrRegister, and ConstantRegister.
 *
 *        Accompanied with the Register class it is possible to perform
 *        operations like:
 *
 *            AndRegister(<register1>,<register2>);
 *
 *        or
 *
 *            AndRegister1 = AndRegister2;
 *
 *        The first example above would return the bitwise AND of register1
 *        and register2.  The second example will assign the value of
 *        AndRegister2 to AndRegister1.
 *
 *        Also, the equality operators "==", "<", and ">=" have been
 *        overloaded to make comparisons easy.  For example:
 *
 *            if (Andregister1 >= Andregister2) {}
 */

namespace libhei
{

/**
 * @brief NotRegister performs a bitwise NOT on a register. For example:
 *
 *            NotReg = NotRegister(<register_name>);
 *
 *        This will apply a bitsize NOT to the register passed in and return
 *        the modified value.
 *
 *        Also implemented are the overloaded operators below:
 *             NotReg =  <register_name>;    (assignment)
 *            (NotReg == <register_name>)    (equals)
 *            (NotReg <  register_name)      (less than)
 *            (NotReg >= register_name)      (greater than or equal to)
**/
class NotRegister : public Register
{
  public:

    /** Constructors */

    /** @brief Default constructor */
    NotRegister() :
        Register( ), iv_child(nullptr), iv_iBS(0)
    {
        iv_bs = &iv_iBS;
    }

    /**
     *  @brief Single parameter constructor.
     *         Sets iv_child, iv_iBS, and iv_bs.
     *  @param i_arg *register
     *  Example:
     *       my_not_register = new NotRegister(*my_not_register);
     */
    NotRegister(Register & i_arg) :
        Register( ), iv_child(&i_arg),
        iv_iBS(i_arg.getSize() * 8)
    {
        iv_bs = &iv_iBS;
    }

    /** @brief Assignment operator */
    NotRegister & operator=(const NotRegister & r)
    {
        iv_child = r.iv_child;
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /**
     *  @brief The line below fixes the following compile error...
     *  error: implicitly-declared 'libhei::NotRegister::
     *  NotRegister(const libhei::NotRegister&)' is deprecated
     *
     *  "The implicit definition of a copy constructor as defaulted is
     *  deprecated if the class has a user-declared copy assignment
     *  operator or a user-declared destructor. The implicit definition
     *  of a copy assignment operator as defaulted is deprecated if the
     *  class has a user-declared copy constructor or a user-declared
     *  destructor (15.4, 15.8). In a future revision of this International
     *  Standard, these implicit definitions could become deleted (11.4)."
     */
    NotRegister(const NotRegister&) = default;

    /** Functions */

    /** @brief Method to get the NOT value of a register. */
    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = ~(*iv_child->getBitString(i_chip));
        return iv_bs;
    }

    /** @brief The size of a register is needed to create a
     *         BitString from a Register?  getSize provides this.
     */
    virtual size_t getSize() const { return (iv_bs->getBitLen() * 8); }

    /** @brief overloaded equality operators */
    bool operator==(const NotRegister & r) const
    { return r.iv_child == iv_child; }

    bool operator<(const NotRegister & r) const
    { return iv_child < r.iv_child; }

    bool operator>=(const NotRegister & r) const
    { return iv_child >= r.iv_child; }

  private:

    /** @brief Required instance variables */
     Register * iv_child;
     BitStringBuffer * iv_bs;
     BitStringBuffer iv_iBS;
};

/**
 * @brief LeftShiftRegister performs a left shift on a register. For example:
 *
 *            LeftShiftReg = LeftShiftRegister(<register_name>);
 *
 *        This will apply a left shift to the register passed in and return
 *        the modified value.
 *
 *        Also implemented are the overloaded operators below:
 *             LeftShiftReg =  <register_name>;    (assignment)
 *            (LeftShiftReg == <register_name>)    (equals)
 *            (LeftShiftReg <  register_name)      (less than)
 *            (LeftShiftReg >= register_name)      (greater or equal to)
 *
**/
class LeftShiftRegister : public Register
{
  public:

    /** Constructors */

    /** @brief Default constructor */
    LeftShiftRegister() :
        Register( ), iv_child(nullptr), iv_amount(0), iv_iBS(0)
    {
        iv_bs = &iv_iBS;
    }

    /**
     *  @brief Two parameter constructor
     *  @param i_arg (register)
     *  @param i_amount (# of bit shifts to perform)
     *  Example:
     *       my_ls_register = new LeftShiftRegister(*my_ls_register);
     */
    LeftShiftRegister(Register & i_arg, uint16_t i_amount) :
        Register( ), iv_child(&i_arg), iv_amount(i_amount),
        iv_iBS(i_arg.getSize() * 8)
    {
        iv_bs = &iv_iBS;
    }

    /** @brief Constructor when class is used via the "=" operator. */
    LeftShiftRegister & operator=(const LeftShiftRegister & r)
    {
        iv_child = r.iv_child;
        iv_amount = r.iv_amount;
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /**
     *  @brief The line below fixes the following compile error:
     *  error: implicitly-declared 'libhei::LeftShiftRegister::
     *  LeftShiftRegister(const libhei::LeftShiftRegister&)' is deprecated
     *
     *  "The implicit definition of a copy constructor as defaulted is
     *  deprecated if the class has a user-declared copy assignment
     *  operator or a user-declared destructor. The implicit definition
     *  of a copy assignment operator as defaulted is deprecated if the
     *  class has a user-declared copy constructor or a user-declared
     *  destructor (15.4, 15.8). In a future revision of this International
     *  Standard, these implicit definitions could become deleted (11.4)."
     */
    LeftShiftRegister(const LeftShiftRegister&) = default;

    /** Functions */

    /** getBitString returns a BitString and performs the "<<" operation. */
    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = (*iv_child->getBitString(i_chip)) << iv_amount;
        return iv_bs;
    }

    virtual size_t getSize() const { return (iv_bs->getBitLen() * 8); }

    /** Overload of the "==" operator */
    bool operator==(const LeftShiftRegister & r) const
    { return (r.iv_child == iv_child) && (r.iv_amount == iv_amount); }

    /** Overload of the "<" operator */
    bool operator<(const LeftShiftRegister & r) const
    {
        if (iv_child == r.iv_child)
            return iv_amount < r.iv_amount;
        return iv_child < r.iv_child;
    }

    /** Overload of the ">=" operator */
    bool operator>=(const LeftShiftRegister & r) const
    {
        if (iv_child == r.iv_child)
            return iv_amount >= r.iv_amount;
        return iv_child >= r.iv_child;
    }

  private:
    Register * iv_child;
    uint16_t iv_amount;

    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};

/**
 * @brief RightShiftRegister performs a right shift on a register.
 *        For example:
 *
 *            RightShiftReg = RightShiftRegister(<register_name>);
 *
 *        This will apply a right shift to the register passed in and return
 *        the modified value.
 *
 *        Also implemented are the overloaded operators below:
 *             RightShiftReg =  <register_name>;    (assignment)
 *            (RightShiftReg == <register_name>)    (equals)
 *            (RightShiftReg <  register_name)      (less than)
 *            (RightShiftReg >= register_name)      (greater or equal to)
 *
**/
class RightShiftRegister : public Register
{
  public:

    /** Constructors */

    /** @brief Default constructor */
    RightShiftRegister() :
        Register( ), iv_child(nullptr), iv_amount(0), iv_iBS(0)
    {
        iv_bs = &iv_iBS;
    }

    /**
     *  @brief Two parameter constructor
     *  @param i_arg (register)
     *  @param i_amount (# of bit shifts to perform)
     *  Example:
     *       my_rs_register = new RightShiftRegister(*my_rs_register,
     *  .                                             i_amount);
     */
    RightShiftRegister(Register & i_arg, uint16_t i_amount) :
        Register( ), iv_child(&i_arg), iv_amount(i_amount),
        iv_iBS(i_arg.getSize() * 8)
    {
        iv_bs = &iv_iBS;
    }

    /** @brief Assignment operator */
    RightShiftRegister & operator=(const RightShiftRegister & r)
    {
        iv_child = r.iv_child;
        iv_amount = r.iv_amount;
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /**
     *  @brief The line below fixes the following compile error:
     *  error: implicitly-declared 'libhei::RightShiftRegister::
     *  RightShiftRegister(const libhei::RightShiftRegister&)' is deprecated
     *
     *  "The implicit definition of a copy constructor as defaulted is
     *  deprecated if the class has a user-declared copy assignment
     *  operator or a user-declared destructor. The implicit definition
     *  of a copy assignment operator as defaulted is deprecated if the
     *  class has a user-declared copy constructor or a user-declared
     *  destructor (15.4, 15.8). In a future revision of this International
     *  Standard, these implicit definitions could become deleted (11.4)."
     */
    RightShiftRegister(const RightShiftRegister&) = default;

    /** Functions */

    /** @brief Method to right shift register. */
    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = (*iv_child->getBitString(i_chip)) >> iv_amount;
        return iv_bs;
    }

    /** @brief The size of a register is needed to create a
     *         BitString from a Register?
     */
    virtual size_t getSize() const { return (iv_bs->getBitLen() * 8); }

    /** overloaded equality operators */
    bool operator==(const RightShiftRegister & r) const
    { return (r.iv_child == iv_child) && (r.iv_amount == iv_amount); }

    bool operator<(const RightShiftRegister & r) const
    {
        if (iv_child == r.iv_child)
            return iv_amount < r.iv_amount;
        return iv_child < r.iv_child;
    }

    bool operator>=(const RightShiftRegister & r) const
    {
        if (iv_child == r.iv_child)
            return iv_amount >= r.iv_amount;
        return iv_child >= r.iv_child;
    }

  private:
    Register * iv_child;
    uint16_t iv_amount;

    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};


/**
 * @brief AndRegister performs a bitwise AND of two registers. For example:
 *
 *            AndReg = AndRegister(<register_name1>, <register_name2>);
 *
 *        This will apply an AND to the registers passed in and return
 *        the result.
 *
 *        Also implemented are the overloaded operators below:
 *             AndReg =  <register_name>;    (assignment)
 *            (AndReg == <register_name>)    (equals)
 *            (AndReg <  register_name)      (less than)
 *            (AndReg >= register_name)      (greater or equal to)
 *
**/
class AndRegister : public Register
{
  public:

    /** Constructors */

    /** @brief Default constructor */
    AndRegister() :
        Register( ), iv_left(nullptr), iv_right(nullptr), iv_iBS(0)
    {
        iv_bs = &iv_iBS;
    }

    /**
     *  @brief Two parameter constructor
     *  @param i_left (register1)
     *  @param i_right (register2)
     *  Example:
     *       my_and_register = new AndRegister(i_left, i_right);
     */
    AndRegister( Register & i_left,
                 Register & i_right ) :
        Register( ), iv_left(&i_left), iv_right(&i_right),
        iv_iBS(std::min(i_left.getSize() * 8,
                        i_right.getSize() * 8))
    {
        iv_bs = &iv_iBS;
    }

    /** @brief Assignment operator */
    AndRegister & operator=(const AndRegister & r)
    {
        iv_left = r.iv_left;
        iv_right = r.iv_right;
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /**
     *  @brief The line below fixes the following compile error:
     *  error: implicitly-declared 'libhei::AndRegister::
     *  AndRegister(const libhei::AndRegister&)' is deprecated
     *
     *  "The implicit definition of a copy constructor as defaulted is
     *  deprecated if the class has a user-declared copy assignment
     *  operator or a user-declared destructor. The implicit definition
     *  of a copy assignment operator as defaulted is deprecated if the
     *  class has a user-declared copy constructor or a user-declared
     *  destructor (15.4, 15.8). In a future revision of this International
     *  Standard, these implicit definitions could become deleted (11.4)."
     */
    AndRegister(const AndRegister&) = default;

    /** Functions */

    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = *iv_left->getBitString(i_chip);
        (*iv_bs) = (*iv_bs) & (*iv_right->getBitString(i_chip));
        return iv_bs;
    }

    /** @brief The size of a register is needed to create a
     *         BitString from a Register?
     */
    virtual size_t getSize() const {return (iv_bs->getBitLen() * 8); };

    /** overloaded equality operators */
    bool operator==(const AndRegister & r) const
    { return (r.iv_left == iv_left) && (r.iv_right == iv_right); }

    bool operator<(const AndRegister & r) const
    {
        if (iv_left == r.iv_left)
            return iv_right < r.iv_right;
        return iv_left < r.iv_left;
    }

    bool operator>=(const AndRegister & r) const
    {
        if (iv_left == r.iv_left)
            return iv_right >= r.iv_right;
        return iv_left >= r.iv_left;
    }

  private:
    Register * iv_left;
    Register * iv_right;

    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};

/**
 * @brief OrRegister performs a bitwise OR between two registers. For example:
 *
 *        my_or_register = new OrRegister(i_left, i_right);
 *
 *        This will apply a bitwise OR on the registers passed in and return
 *        the modified value.
 *
 *        Also implemented are the overloaded operators below:
 *             AndReg =  <register_name>;    (assignment)
 *            (AndReg == <register_name>)    (equals)
 *            (AndReg <  register_name)      (less than)
 *            (AndReg >= register_name)      (greater or equal to)
 *
**/
class OrRegister : public Register
{
  public:

    /** @brief Default constructor */
    OrRegister() :
        Register( ), iv_left(nullptr), iv_right(nullptr), iv_iBS(0)
    {
        iv_bs = &iv_iBS;
    }

    /**
     *  @brief Two parameter constructor
     *  @param i_left (register1)
     *  @param i_right (register2)
     *  Example:
     *       my_or_register = new OrRegister(i_left, i_right);
     */
    OrRegister( Register & i_left,
                Register & i_right ) :
        Register( ), iv_left(&i_left), iv_right(&i_right),
        iv_iBS(std::min(i_left.getSize() * 8,
                        i_right.getSize() * 8))
    {
        iv_bs = &iv_iBS;
    }

    /** @brief Constructor when class is used via the "=" operator. */
    OrRegister & operator=(const OrRegister & r)
    {
        iv_left = r.iv_left;
        iv_right = r.iv_right;
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /**
     *  @brief The line below fixes the following compile error:
     *  error: implicitly-declared 'libhei::OrRegister::
     *  OrRegister(const libhei::OrRegister&)' is deprecated
     *
     *  "The implicit definition of a copy constructor as defaulted is
     *  deprecated if the class has a user-declared copy assignment
     *  operator or a user-declared destructor. The implicit definition
     *  of a copy assignment operator as defaulted is deprecated if the
     *  class has a user-declared copy constructor or a user-declared
     *  destructor (15.4, 15.8). In a future revision of this International
     *  Standard, these implicit definitions could become deleted (11.4)."
     */
    OrRegister(const OrRegister&) = default;

    /** Functions */

    /** getBitString returns a BitString and performs the "|" (OR) operation. */
    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = *iv_left->getBitString(i_chip);
        (*iv_bs) = (*iv_bs) | (*iv_right->getBitString(i_chip));
        return iv_bs;
    }

    /** Store away size so we can build a bitstring */ 
    virtual size_t getSize() const {return (iv_bs->getBitLen() * 8); }

    /** Overload of the "==" operator */
    bool operator==(const OrRegister & r) const
    { return (r.iv_left == iv_left) && (r.iv_right == iv_right); }

    /** Overload of the "<" operator */
    bool operator<(const OrRegister & r) const
    {
        if (iv_left == r.iv_left)
            return iv_right < r.iv_right;
        return iv_left < r.iv_left;
    }

    /** Overload of the ">=" operator */
    bool operator>=(const OrRegister & r) const
    {
        if (iv_left == r.iv_left)
            return iv_right >= r.iv_right;
        return iv_left >= r.iv_left;
    }

  private:
    Register * iv_left;
    Register * iv_right;

    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};

/**
 * @brief ConstantRegister holds a value assigned to it.  For example:
 *
 *        ConstantRegister constreg = AndRegister(<register_name>);
 *
 *        This to the register passed in and return
 *        the modified value.
 *
 *        Also implemented are the overloaded operators below:
 *             AndReg =  <register_name>;    (assignment)
 *            (AndReg == <register_name>)    (equals)
 *            (AndReg <  register_name)      (less than)
 *            (AndReg >= register_name)      (greater or equal to)
 *
**/
class ConstantRegister : public Register
{
  public:

    /** @brief Default constructor */
    ConstantRegister() :
        Register( ), iv_iBS(0)
    {}

    /**
     *  @brief Single parameter constructor.
     *         Sets iv_iBS
     *  @param BitStringBuffer i_arg 
     *  Example:
     *       constant_register = new ConstantRegister(i_arg);
     */
    ConstantRegister( const BitStringBuffer & i_arg ) :
        Register( ), iv_iBS(i_arg)
    {}

    /** @brief Constructor when class is used via the "=" operator. */
    ConstantRegister & operator=(const ConstantRegister & r)
    {
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /**
     *  @brief The line below fixes the following compile error:
     *  error: implicitly-declared 'libhei::ConstantRegister::
     *  ConstantRegister(const libhei::ConstantRegister&)' is deprecated
     *
     *  "The implicit definition of a copy constructor as defaulted is
     *  deprecated if the class has a user-declared copy assignment
     *  operator or a user-declared destructor. The implicit definition
     *  of a copy assignment operator as defaulted is deprecated if the
     *  class has a user-declared copy constructor or a user-declared
     *  destructor (15.4, 15.8). In a future revision of this International
     *  Standard, these implicit definitions could become deleted (11.4)."
     */
    ConstantRegister(const ConstantRegister&) = default;

    /** Functions */

    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        return &iv_iBS;
    }

    virtual size_t getSize() const {return (iv_iBS.getBitLen() * 8); };

    bool operator==(const ConstantRegister & r) const
    { return r.iv_iBS == iv_iBS; }

  private:
    BitStringBuffer iv_iBS;
};

} // end namespace libhei

