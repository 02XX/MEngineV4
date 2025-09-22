// #include "GraphicPipeline.hpp"
// #include "AssetManager.hpp"
// #include "GBufferPipelineBuilder.hpp"
// #include "RHIContext.hpp"
// #include "RHIResource.hpp"
// #include "ShaderBuilder.hpp"
// #include "gtest/gtest.h"
// #include <gtest/gtest.h>
// #include <memory>
// #include <vcruntime_typeinfo.h>

// using namespace MEngine::Resource;
// class GraphicPipelineTest : public ::testing::Test
// {
//   protected:
//     std::shared_ptr<AssetManager> assetManager;
//     void SetUp() override
//     {
//         auto &rhiContext = MEngine::Platform::RHIContext::Instance();
//         RHIContextConfig config;
//         config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
//         rhiContext.InitInstance(config);
//         rhiContext.InitContext();
//         assetManager = std::make_shared<AssetManager>();
//         ShaderBuilder builder(assetManager);
//         builder.SetGLSL(R"(
// #version 450
// layout(location = 0) in vec3 inPosition;
// layout(location = 1) in vec3 inNormal;
// layout(location = 2) in vec2 inTexCoord;

// layout(location = 0) out vec3 outNormal;
// layout(location = 1) out vec2 outTexCoord;

// void main() {
//     gl_Position = vec4(inPosition, 1.0);
//     outNormal = inNormal;
//     outTexCoord = inTexCoord;
// }
// )");
//         builder.SetStage(vk::ShaderStageFlagBits::eVertex);
//         builder.SetName("gbuffer_vert");
//         auto shader = builder.Build();
//         shader->GetResource()->InitResource();
//         assetManager->AddAsset(std::move(shader));
//         builder.Reset();
//         builder.SetGLSL(R"(
// #version 450
// layout(location = 0) in vec3 inNormal;
// layout(location = 1) in vec2 inTexCoord;

// layout(location = 0) out vec4 outAlbedo;
// layout(location = 1) out vec4 outNormal;
// layout(location = 2) out vec4 outMaterial;

// void main() {
//     outAlbedo = vec4(1.0, 1.0, 1.0, 1.0);
//     outNormal = vec4(normalize(inNormal), 1.0);
//     outMaterial = vec4(0.5, 0.5, 0.5, 1.0);
// }
// )");
//         builder.SetStage(vk::ShaderStageFlagBits::eFragment);
//         builder.SetName("gbuffer_frag");
//         shader = builder.Build();
//         shader->GetResource()->InitResource();
//         assetManager->AddAsset(std::move(shader));
//     }
//     void TearDown() override
//     {
//         assetManager.reset();
//         GTEST_LOG_(INFO) << "PendingDeletes Size: " << PendingDeletes.Size();
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//         while (PendingDeletes.Size() > 0)
//         {
//             auto resource = PendingDeletes.Consume();
//             GTEST_LOG_(INFO) << "Deleting pending resource." << typeid(*resource).name();
//             delete resource;
//         }
//     }
// };

// TEST_F(GraphicPipelineTest, CreateGBufferPipeline)
// {
//     GBufferPipelineBuilder builder(assetManager);
//     builder.SetInputAssemblyState();
//     builder.SetShader();
//     builder.SetColorBlendState();
//     builder.SetRasterizationState();
//     builder.SetMultiSampleState();
//     builder.SetDepthStencilState();
//     builder.SetColorAttachmentFormats();
//     builder.SetDepthStencilAttachmentFormat();
//     builder.SetLayout();
//     builder.SetName("GBufferPipeline");
//     auto pipeline = builder.Build();
//     pipeline->GetResource()->InitResource();
//     EXPECT_NE(pipeline, nullptr);
//     pipeline->GetResource()->ReleaseResource();
// }
