#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <memory>
#include <typeindex>

class Base
{
  public:
    virtual ~Base() = default;
};
class DerivedA : public Base
{
  public:
    ~DerivedA() override = default;
};
class DerivedB : public Base
{
  public:
    ~DerivedB() override = default;
};

class DispatcherTest : public ::testing::Test
{
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(DispatcherTest, dispatch)
{
    std::shared_ptr<Base> asset = std::make_shared<DerivedA>();
    auto &obj = *asset;
    auto type = std::type_index(typeid(obj));
    GTEST_LOG_(INFO) << type.name();
}