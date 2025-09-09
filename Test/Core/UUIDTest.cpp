#include "UUID.hpp"
#include "UUIDGenerator.hpp"
#include <gtest/gtest.h>
using namespace MEngine::Core;
TEST(UUIDTest, GenerateUUID)
{
    auto id1 = UUIDGenerator::Instance().Create();
    EXPECT_NE(id1, UUID{});
}
TEST(UUIDTest, ASEND)
{
    auto id1 = UUIDGenerator::Instance().Create();
    auto id2 = UUIDGenerator::Instance().Create();
    EXPECT_LE(id1, id2);
}
TEST(UUIDTest, EQUAL)
{
    auto id1 = UUIDGenerator::Instance().Create();
    auto id2 = id1;
    EXPECT_EQ(id1, id2);
}
TEST(UUIDTest, NOT_EQUAL)
{
    auto id1 = UUIDGenerator::Instance().Create();
    auto id2 = UUIDGenerator::Instance().Create();
    EXPECT_NE(id1, id2);
}
TEST(UUIDTest, TO_STRING)
{
    auto id1 = UUIDGenerator::Instance().Create();
    auto str = id1.ToString();
    EXPECT_EQ(str.size(), 36);
}
TEST(UUIDTest, FROM_STRING)
{
    auto id1 = UUIDGenerator::Instance().Create();
    auto str = id1.ToString();
    auto id2 = UUID(str);
    EXPECT_EQ(id1, id2);
}