// clang-format off
#pragma once

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
 *        Also, the equality operators "==", and "<" have been
 *        overloaded to make comparisons easy.  For example:
 *
 *            if (Andregister1 < Andregister2) {}
 */
namespace libhei
{
/**
 * @brief NotRegister performs a bitwise NOT on a register. For example:
 *
 *            NotReg = NotRegister(<register_name>);
 *
 *        This will apply a bitwize NOT to the register passed in and return
 *        the modified value.
 *
 *        Also implemented are the overloaded operators below:
 *             NotReg =  <register_name>;    (assignment)
 *            (NotReg == <register_name>)    (equals)
 *            (NotReg <  register_name)      (less than)
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
     *         Initializes iv_child, iv_iBS, and iv_bs.
     *  @param i_arg (register)
     */
    NotRegister(Register & i_arg) :
        Register( ), iv_child(&i_arg),
        iv_iBS(i_arg.getSize()/8)
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

    /** Explicitly declare the following so they aren't mis-used. */
    ~NotRegister() = default;
    NotRegister(const NotRegister &) = default;

    /** Functions */

    /**
     *  @brief  Creates a bitstring object from a Chip type and performs
     *          the NOT operation.
     *  @param  Chip object
     *  @return A bitstring object
     */
    virtual const BitString * getBitString(const Chip & i_chip) const
    {
        (*iv_bs) = ~(*iv_child->getBitString(i_chip));
        return iv_bs;
    }

    /** @return The size of the register in bytes. */
    virtual size_t getSize() const
    { 
        return (BitString::getMinBytes(iv_iBS.getBitLen()));
    }

    /** overloaded equality operators ("==" and "<") */

    bool operator==(const NotRegister & r) const
    { return iv_iBS == r.iv_iBS; }

    bool operator<(const NotRegister & r) const
    { return iv_child < r.iv_child; }

  private:

    /** Register representing the value to NOT. */
    Register * iv_child;

    /** BitStrings used to carry out the NOT functionality. */
    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};

/**
 * @brief LeftShiftRegister performs a left shift on a register. For example:
 *
 *        LeftShiftReg = LeftShiftRegister(<register_name>,<# of bits>);
 *
 *        This will apply a left shift to the register passed in and return
 *        the modified value.
 *
 *        Also implemented are the overloaded operators below:
 *             LeftShiftReg =  <register_name>;    (assignment)
 *            (LeftShiftReg == <register_name>)    (equals)
 *            (LeftShiftReg <  register_name)      (less than)
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
     */
    LeftShiftRegister(Register & i_arg, uint16_t i_amount) :
        Register( ), iv_child(&i_arg), iv_amount(i_amount),
        iv_iBS(i_arg.getSize())
    {
        iv_bs = &iv_iBS;
    }

    /** @brief Assignment operator */
    LeftShiftRegister & operator=(const LeftShiftRegister & r)
    {
        iv_child = r.iv_child;
        iv_amount = r.iv_amount;
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /** Explicitly declare the following so they aren't mis-used. */
    ~LeftShiftRegister() = default;
    LeftShiftRegister(const LeftShiftRegister &) = default;

    /** Functions */

    /**
     *  @brief  Creates a bitstring object from a Chip type and performs
     *          the Left Shift operation.
     *  @param  Chip object
     *  @return A bitstring object
     */
    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = (*iv_child->getBitString(i_chip)) << iv_amount;
        return iv_bs;
    }

    /** @return The size of the register in bytes. */
    virtual size_t getSize() const 
    {
        return (BitString::getMinBytes(iv_iBS.getBitLen()));
    }

    /** overloaded equality operators ("==" and "<") */

    bool operator==(const LeftShiftRegister & r) const
    { return (r.iv_child == iv_child) && (r.iv_amount == iv_amount); }

    bool operator<(const LeftShiftRegister & r) const
    {
        if (iv_child == r.iv_child)
            return iv_amount < r.iv_amount;
        return iv_child < r.iv_child;
    }

  private:
    /** Register to perform the shift left upon. */
    Register * iv_child;
    /** Number of bits to shift */
    uint16_t iv_amount;

    /** BitStrings used to carry out the left shift functionality. */
    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};

/**
 * @brief RightShiftRegister performs a right shift on a register.
 *      For example:
 *
 *      RightShiftReg = RightShiftRegister(<register_name>,# of bits to shift);
 *
 *      This will apply a right shift to the register passed in and return
 *      the modified value.
 *
 *      Also implemented are the overloaded operators below:
 *           RightShiftReg =  <register_name>;    (assignment)
 *          (RightShiftReg == <register_name>)    (equals)
 *          (RightShiftReg <  register_name)      (less than)
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
     */
    RightShiftRegister(Register & i_arg, uint16_t i_amount) :
        Register( ), iv_child(&i_arg), iv_amount(i_amount),
        iv_iBS(i_arg.getSize())
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

    /** Explicitly declare the following so they aren't mis-used. */
    ~RightShiftRegister() = default;
    RightShiftRegister(const RightShiftRegister &) = default;

    /** Functions */

    /**
     *  @brief  Creates a bitstring object from a Chip type and performs
     *          the Right Shift operation.
     *  @param  Chip object
     *  @return A bitstring object
     */
    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = (*iv_child->getBitString(i_chip)) >> iv_amount;
        return iv_bs;
    }

    /** @return The size of a register in bytes. */
    virtual size_t getSize() const
    { 
        return (BitString::getMinBytes(iv_iBS.getBitLen()));
    }

    /** overloaded equality operators ("==" and "<") */

    bool operator==(const RightShiftRegister & r) const
    { return (r.iv_child == iv_child) && (r.iv_amount == iv_amount); }

    bool operator<(const RightShiftRegister & r) const
    {
        if (iv_child == r.iv_child)
            return iv_amount < r.iv_amount;
        return iv_child < r.iv_child;
    }

  private:
    /** Register to perform the shift right upon. */
    Register * iv_child;

    /** Number of bits to shift */
    uint16_t iv_amount;

    /** BitStrings used to carry out the right shift functionality. */
    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};


/**
 * @brief AndRegister performs a bitwise AND of two registers. For example:
 *
 *            AndReg = AndRegister(<register1>, <register2>);
 *
 *        This will apply an AND to the registers passed in and return
 *        the result.
 *
 *        Also implemented are the overloaded operators below:
 *             AndReg =  <register_name>;    (assignment)
 *            (AndReg == <register_name>)    (equals)
 *            (AndReg <  register_name)      (less than)
 *
**/
class AndRegister : public Register
{
  public:

    /* Constructors */

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
     */
    AndRegister( Register & i_left,
                 Register & i_right ) :
        Register( ), iv_left(&i_left), iv_right(&i_right),
        iv_iBS(std::min(i_left.getSize(),
                        i_right.getSize()))
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

    /** Explicitly declare the following so they aren't mis-used. */
    ~AndRegister() = default;
    AndRegister(const AndRegister &) = default;

    /* Functions */

    /**
     *  @brief  Creates a bitstring object from a Chip type and performs
     *          the AND operation.
     *  @param  Chip object
     *  @return A bitstring object
     */
    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = *iv_left->getBitString(i_chip);
        (*iv_bs) = (*iv_bs) & (*iv_right->getBitString(i_chip));
        return iv_bs;
    }

    /** @return The size of the register in bytes. */
    virtual size_t getSize() const
    { 
        return (BitString::getMinBytes(iv_iBS.getBitLen()));
    }

    /** overloaded equality operators ("==" and "<") */

    bool operator==(const AndRegister & r) const
    { 
        return (iv_iBS == r.iv_iBS);
    }

    bool operator<(const AndRegister & r) const
    {
        if (r.iv_left == iv_left)
            return r.iv_right < iv_right;
        return r.iv_left < iv_left;
    }

  private:
    /** Registers representing the two values to AND. */
    Register * iv_left;
    Register * iv_right;

    /** BitStrings used to carry out the AND functionality. */
    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};

/**
 * @brief OrRegister performs a bitwise OR between two registers. For example:
 *
 *        This will apply a bitwise OR on the registers passed in and return
 *        the modified value.
 *
 *        Also implemented are the overloaded operators below:
 *             AndReg =  <register_name>;    (assignment)
 *            (AndReg == <register_name>)    (equals)
 *            (AndReg <  register_name)      (less than)
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
     */
    OrRegister( Register & i_left,
                Register & i_right ) :
        Register( ), iv_left(&i_left), iv_right(&i_right),
        iv_iBS(std::min(i_left.getSize(),
                        i_right.getSize()))
    {
        iv_bs = &iv_iBS;
    }

    /** @brief Assignment operator */
    OrRegister & operator=(const OrRegister & r)
    {
        iv_left = r.iv_left;
        iv_right = r.iv_right;
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /** Explicitly declare the following so they aren't mis-used. */
    ~OrRegister() = default;
    OrRegister(const OrRegister &) = default;

    /** Functions */

    /**
     *  @brief  Creates a bitstring object from a Chip type and performs
     *          the OR operation.
     *  @param  Chip object
     *  @return A bitstring object
     */
    virtual const BitString * getBitString( const Chip & i_chip ) const
    {
        (*iv_bs) = *iv_left->getBitString(i_chip);
        (*iv_bs) = (*iv_bs) | (*iv_right->getBitString(i_chip));
        return iv_bs;
    }

    /** @return The size of the register in bytes. */
    virtual size_t getSize() const
    { 
        return (BitString::getMinBytes(iv_iBS.getBitLen()));
    }

    /** overloaded equality operators ("==" and "<") */

    bool operator==(const OrRegister & r) const
        { return (iv_iBS == r.iv_iBS); }

    bool operator<(const OrRegister & r) const
    {
        if (iv_left == r.iv_left)
            return r.iv_right < iv_right;
        return r.iv_left < iv_left;
    }

  private:
    /** Registers representing the two values to OR. */
    Register * iv_left;
    Register * iv_right;

    /** BitStrings used to carry out the AND functionality. */
    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};

/**
 * @brief ConstantRegister holds a value assigned to it.  The value
 *        can be retrieved using the getBitString function.
 *
 *        Also implemented are the overloaded operators below:
 *             AndReg =  <register_name>;    (assignment)
 *            (AndReg == <register_name>)    (equals)
 *            (AndReg <  register_name)      (less than)
 *
**/
class ConstantRegister : public Register
{
  public:

    /* Constructors */

    /** @brief Default constructor */
    ConstantRegister() :
        Register( ), iv_iBS(0)
    {}

    /**
     *  @brief Single parameter constructor.
     *         Sets iv_iBS
     *  @param i_arg (BitStringBuffer)
     */
    ConstantRegister(const BitStringBuffer & i_arg) :
        Register( ), iv_iBS(i_arg)
    {}

    /** @brief Assignment operator */
    ConstantRegister & operator=(const ConstantRegister & r)
    {
        iv_child = r.iv_child;
        iv_iBS = r.iv_iBS;
        return *this;
    }

    /** Explicitly declare the following so they aren't mis-used. */
    ~ConstantRegister() = default;
    ConstantRegister(const ConstantRegister &) = default;

    /** Functions */

    /**
     *  @brief  Creates a bitstring object from a Chip type
     *  @param  Chip object
     *  @return A bitstring object
     */
    virtual const BitString * getBitString(const Chip & i_chip) const
    {
        return &iv_iBS;
    }

    /** @return The size of the register in bytes. */
    virtual size_t getSize() const
    {
        return (BitString::getMinBytes(iv_iBS.getBitLen()));
    }

    /** overloaded equality operator ("==") */

    bool operator==(const ConstantRegister & r) const
    { return r.iv_iBS == iv_iBS; }

    bool operator<(const ConstantRegister & r) const
    { return r.iv_child < iv_child; }

  private:
    /** Working Register. */
    Register * iv_child;

    /** BitStrings used to carry out the ConstantRegister functionality. */
    BitStringBuffer * iv_bs;
    BitStringBuffer iv_iBS;
};

} // end namespace libhei
// clang-format on
