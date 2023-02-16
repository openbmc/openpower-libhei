#include <hei_chip.hpp>
#include <register/hei_operator_register.hpp>
#include <util/hei_flyweight.hpp>

#include "gtest/gtest.h"

using namespace libhei;

constexpr uint16_t CONST1 = 0x1458;
constexpr uint16_t CONST2 = 0x5368;

#define MATH(x) static_cast<uint16_t>(x)

uint64_t __getVal(Register::ConstPtr i_reg)
{
    Chip CHIP{nullptr, 0};
    auto bs = i_reg->getBitString(CHIP);
    return bs->getFieldRight(0, bs->getBitLen());
}

size_t __getBW(Register::ConstPtr i_reg)
{
    Chip CHIP{nullptr, 0};
    return i_reg->getBitString(CHIP)->getBitLen();
}

TEST(OperatorRegisterTest, BasicOperations)
{
    auto const1 = std::make_shared<const ConstantRegister>(CONST1);
    ASSERT_EQ(MATH(CONST1), __getVal(const1));

    auto const2 = std::make_shared<const ConstantRegister>(CONST2);
    ASSERT_EQ(MATH(CONST2), __getVal(const2));

    auto and1 = std::make_shared<const AndRegister>(const1, const2);
    ASSERT_EQ(MATH(CONST1 & CONST2), __getVal(and1));

    auto or1 = std::make_shared<const OrRegister>(const1, const2);
    ASSERT_EQ(MATH(CONST1 | CONST2), __getVal(or1));

    auto not1 = std::make_shared<const NotRegister>(const1);
    ASSERT_EQ(MATH(~CONST1), __getVal(not1));

    auto not2 = std::make_shared<const NotRegister>(const2);
    ASSERT_EQ(MATH(~CONST2), __getVal(not2));
}

TEST(OperatorRegisterTest, BasicOperationsWithFlyweights)
{
    auto const1 = Flyweight<const ConstantRegister>::get(CONST1);
    auto const2 = Flyweight<const ConstantRegister>::get(CONST2);

    ASSERT_EQ(MATH(CONST1), __getVal(const1));
    ASSERT_EQ(MATH(CONST2), __getVal(const2));
    ASSERT_EQ(MATH(CONST1 & CONST2),
              __getVal(Flyweight<const AndRegister>::get(const1, const2)));
    ASSERT_EQ(MATH(CONST1 | CONST2),
              __getVal(Flyweight<const OrRegister>::get(const1, const2)));
    ASSERT_EQ(MATH(~CONST1),
              __getVal(Flyweight<const NotRegister>::get(const1)));
    ASSERT_EQ(MATH(~CONST2),
              __getVal(Flyweight<const NotRegister>::get(const2)));

    Flyweight<const ConstantRegister>::clear();
    Flyweight<const AndRegister>::clear();
    Flyweight<const OrRegister>::clear();
    Flyweight<const NotRegister>::clear();
}

TEST(OperatorRegisterTest, ShiftOperations)
{
    auto const1 = Flyweight<const ConstantRegister>::get(CONST1);

    for (size_t i = 0; i < __getBW(const1); i++)
    {
        ASSERT_EQ(MATH(CONST1 << i),
                  __getVal(Flyweight<const LeftShiftRegister>::get(const1, i)));
    }

    for (size_t i = 0; i < __getBW(const1); i++)
    {
        ASSERT_EQ(
            MATH(CONST1 >> i),
            __getVal(Flyweight<const RightShiftRegister>::get(const1, i)));
    }

    Flyweight<const ConstantRegister>::clear();
    Flyweight<const LeftShiftRegister>::clear();
    Flyweight<const RightShiftRegister>::clear();
}

TEST(OperatorRegisterTest, ComplexOperation)
{
    // Something seemingly complex:
    //   ~(((CONST1 & CONST2) << 12) | ((CONST1 | CONST2) >> 4)))

    auto const1 = Flyweight<const ConstantRegister>::get(CONST1);
    auto const2 = Flyweight<const ConstantRegister>::get(CONST2);

    auto lshift = Flyweight<const LeftShiftRegister>::get(
        Flyweight<const AndRegister>::get(const1, const2), 12);
    auto rshift = Flyweight<const RightShiftRegister>::get(
        Flyweight<const OrRegister>::get(const1, const2), 4);

    auto expr = Flyweight<const NotRegister>::get(
        Flyweight<const OrRegister>::get(lshift, rshift));

    ASSERT_EQ(MATH(~(((CONST1 & CONST2) << 12) | ((CONST1 | CONST2) >> 4))),
              __getVal(expr));

    Flyweight<const ConstantRegister>::clear();
    Flyweight<const AndRegister>::clear();
    Flyweight<const OrRegister>::clear();
    Flyweight<const NotRegister>::clear();
    Flyweight<const LeftShiftRegister>::clear();
    Flyweight<const RightShiftRegister>::clear();
}

TEST(OperatorRegisterTest, ConstRegConstructor)
{
    // The ConstantRegister constructor is a template that requires some integer
    // type.

    // CONST1 is uint16_t, GOOD
    auto const1 = Flyweight<const ConstantRegister>::get(CONST1);

    // This also works
    auto const2 = Flyweight<const ConstantRegister>::get(uint16_t{0x1458});

    ASSERT_EQ(const1, const2); // Should point to the same thing.

    // If you pass in a constant with no type the compiler assumes it is the
    // implicit int type. This value is the same as const2, but the size will be
    // different.
    auto const3 = Flyweight<const ConstantRegister>::get(0x1458);

    // The values are the same, but the bit strings are are different sizes.
    ASSERT_NE(const2, const3);

    // AND and OR operators will not like different sizes either.
    ASSERT_DEATH(Flyweight<const AndRegister>::get(const2, const3), "");

    Flyweight<const ConstantRegister>::clear();
    Flyweight<const AndRegister>::clear();
}
