#include <gtest/gtest.h>
#include <print>

class Base
{
  public:
    Base(int a)
    {
        std::println("Base constructor with int: {}", a);
    }
    virtual ~Base() = default;
    virtual void Dummy()
    {
        std::println("Base Dummy");
    }
};

class Derived : public Base
{
    using Base::Base;

  public:
    void Dummy() override
    {
        std::println("Derived Dummy");
    }
};
class AnotherDerived : public Base
{
  public:
    void Dummy() override
    {
        std::println("AnotherDerived Dummy");
    }
};

TEST(BasicTest, SampleTest)
{
    Base *base = new Derived(42);
    base->Dummy();
    delete base;
}