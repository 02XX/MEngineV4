#include "AssetManager.hpp"
#include "RHIContext.hpp"
#include <Texture2D.hpp>
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Resource;
class AssetManagerTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};
TEST(AssetManagerTest, Destroy)
{
    auto assetManager = std::make_shared<AssetManager>();
}
