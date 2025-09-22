#include "Texture2DResource.hpp"
#include "RHIContext.hpp"
#include "RHIHandler.hpp"
#include "Texture2D.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine::Resource;
class Texture2DResourceTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto &rhiContext = MEngine::Platform::RHIContext::Instance();
        RHIContextConfig config;
        config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
        config.DeviceRequiredExtensions = {"VK_EXT_host_image_copy"};
        rhiContext.InitInstance(config);
        rhiContext.InitContext();
    }
    void TearDown() override
    {
    }
};

TEST_F(Texture2DResourceTest, CreateTexture2DResource)
{
    // auto texture2D = std::make_unique<Texture2D>();
    // auto texture2DResource = std::make_unique<Texture2DResource>(texture2D.get());
    // EXPECT_NE(texture2DResource, nullptr);
    // texture2DResource->InitRHI();
    // texture2DResource->ReleaseRHI();
    // EXPECT_NE(PendingDeletes.Size(), 0);
    // while (PendingDeletes.Size() > 0)
    // {
    //     auto resource = PendingDeletes.Consume();
    //     resource->Release();
    // }
}