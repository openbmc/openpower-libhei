#include <util/hei_flyweight.hpp>

#include "gtest/gtest.h"

using namespace libhei;

class Foo
{
  public:
    explicit Foo(int i) : iv_i(i) {}

    bool operator==(const Foo& i_r) const
    {
        return iv_i == i_r.iv_i;
    }

    bool operator<(const Foo& i_r) const
    {
        return iv_i < i_r.iv_i;
    }

  private:
    int iv_i = 0;
};

TEST(FlyweightTest, TestSet1)
{
    auto f1 = Flyweight<Foo>::get(1);
    auto f2 = Flyweight<Foo>::get(2);
    auto f3 = Flyweight<Foo>::get(1); // same as f1

    ASSERT_NE(f1, f2); // Pointing to different objects
    ASSERT_EQ(f1, f3); // Pointing to the same object

    ASSERT_EQ(size_t(2),
              Flyweight<Foo>::size()); // Only two entries in the flyweight
}
