// #include "Shader.hpp"
// #include "RHIContext.hpp"
// #include "RHIResource.hpp"
// #include "ShaderBuilder.hpp"
// #include "ShaderResource.hpp"
// #include <gtest/gtest.h>

// using namespace MEngine::Resource;
// TEST(ShaderTest, CreateShader)
// {
//     auto assetManager = std::make_shared<AssetManager>();
//     auto &rhiContext = MEngine::Platform::RHIContext::Instance();
//     RHIContextConfig config;
//     config.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
//     rhiContext.InitInstance(config);
//     rhiContext.InitContext();
//     ShaderBuilder builder(assetManager);
//     builder.SetGLSL("#version 450\nvoid main(){}");
//     builder.SetStage(vk::ShaderStageFlagBits::eVertex);
//     auto shader = builder.Build();
//     shader->GetResource()->InitResource();
//     EXPECT_NE(shader, nullptr);
//     EXPECT_TRUE(static_cast<ShaderResource *>(shader->GetResource())->GetRHIShader()->GetShaderModule() != nullptr);
//     shader->GetResource()->ReleaseResource();
//     auto resource = PendingDeletes.Consume();
//     delete resource;
// }