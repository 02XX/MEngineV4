#include "RflEntity.hpp"

#include <memory>

#include "Context.hpp"
#include "Shader.hpp"
#include "ShaderManager.hpp"
#include "UUID.hpp"
#include <gtest/gtest.h>
#include <print>

using namespace MEngine::Resource;
class SerializationTest : public ::testing::Test
{
  protected:
    std::shared_ptr<Context> mContext;
    void SetUp() override
    {
        Logger::GetInstance().GetLogger()->SetLogLevel(LogLevel::Trace);
        ContextConfig config{};
        config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
        mContext = std::make_shared<Context>(config);
        AssetManager::Instance().Init(mContext);
    }

    void TearDown() override
    {
    }
};

TEST_F(SerializationTest, ShaderAsset)
{
    auto &assetManager = AssetManager::Instance();
    auto shader = std::make_shared<Shader>("TestShader",
                                           std::vector<uint32_t>{
                                               0x03022307,
                                               0x0000000b,
                                               0x0008000a,
                                               0x00000005,
                                               0x00000000,
                                               0x20011,
                                               0x00000001,
                                               0x6000b,
                                               0x00000002,
                                               0x4c534c47,
                                               0x6474732e,
                                               0x3035342e,
                                               0x00000000,
                                               0x0003000e,
                                               0x00000000,
                                               0x00000001,
                                           },
                                           vk::ShaderStageFlagBits::eVertex);
    auto assetEntity = AssetEntity{
        .id = shader->mID,
        .name = shader->mName,
    };
    ShaderEntity shaderEntity{
        .assetEntity = assetEntity,
        .stage = shader->mStage,
        .spirvCode = shader->mSPIRVCode,
    };
    assetManager.Save(shader, AssetURL("shader://TestShader.shader"));
}