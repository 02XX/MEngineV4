#pragma once
#include "Context.hpp"
#include "GraphicPipeline.hpp"
#include "Manager.hpp"
#include "PendingResourceManager.hpp"
#include "PipelineResource.hpp"
#include "RflEntity.hpp"
#include "ShaderManager.hpp"
#include <concepts>
#include <memory>
#include <sys/stat.h>
#include <unordered_map>
#include <vulkan/vulkan_structs.hpp>

namespace MEngine::Resource
{
struct DefaultGraphicPipelineType
{
    static constexpr const char *ForwardOpaquePhong = "GraphicPipeline_Forward_Opaque_Phong";
    static constexpr const char *ForwardTransparentPhong = "GraphicPipeline_Forward_Transparent_Phong";
    static constexpr const char *ForwardOpaquePBR = "GraphicPipeline_Forward_Opaque_PBR";
    static constexpr const char *GBufferOpaquePBR = "GraphicPipeline_GBuffer_Opaque_PBR";
    static constexpr const char *LightingOpaquePBR = "GraphicPipeline_Lighting_Opaque_PBR";
    static constexpr const char *Skybox = "GraphicPipeline_Skybox";
    static constexpr const char *PostProcess = "GraphicPipeline_PostProcess";
    static constexpr const char *UI = "GraphicPipeline_UI";
};
struct DefaultComputePipelineType
{
};
struct DefaultRayTracingPipelineType
{
};
struct DefaultDescriptorSetLayoutType
{
    static constexpr const char *Global = "DescriptorSetLayout_Global";
    static constexpr const char *Material = "DescriptorSetLayout_Material";
    static constexpr const char *Bindless = "DescriptorSetLayout_Bindless";
};
struct DefaultPushConstantRangeType
{
    static constexpr const char *Matrix = "PushConstantRange_Matrix";
};
constexpr static uint32_t MAX_DESCRIPTOR_COUNT = 1024;
class PipelineManager final : public Manager, PendingResourceManager<PipelineResource>
{
  private:
    std::shared_ptr<ShaderManager> mShaderManager;

  public:
    static inline const std::unordered_map<std::string, Core::UUID> sDefaultPipelines{
        {DefaultGraphicPipelineType::ForwardOpaquePhong, Core::UUID{"20000000-0000-0000-0000-000000000000"}},
        {DefaultGraphicPipelineType::ForwardTransparentPhong, Core::UUID{"20000000-0000-0000-0000-000000000001"}},
        {DefaultGraphicPipelineType::ForwardOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000002"}},
        {DefaultGraphicPipelineType::GBufferOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000003"}},
        {DefaultGraphicPipelineType::LightingOpaquePBR, Core::UUID{"20000000-0000-0000-0000-000000000004"}},
        {DefaultGraphicPipelineType::Skybox, Core::UUID{"20000000-0000-0000-0000-000000000005"}},
        {DefaultGraphicPipelineType::PostProcess, Core::UUID{"20000000-0000-0000-0000-000000000006"}},
        {DefaultGraphicPipelineType::UI, Core::UUID{"20000000-0000-0000-0000-000000000007"}},
    };
    static inline const std::vector<vk::DescriptorPoolSize> sDescriptorPoolSize{
        vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, MAX_DESCRIPTOR_COUNT},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, MAX_DESCRIPTOR_COUNT},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, MAX_DESCRIPTOR_COUNT}};
    static inline const std::vector<vk::Format> sMRTFormats{
        vk::Format::eR32G32B32A32Sfloat, // Color Attachment 0 Color+Albedo
        vk::Format::eR32G32B32A32Sfloat, // Color Attachment 1 Normal
        vk::Format::eR32G32B32A32Sfloat, // Color Attachment 2 ARM AO+Roughness+Metallic
        vk::Format::eR32G32B32A32Sfloat, // Color Attachment 3 Position
        vk::Format::eR32G32B32A32Sfloat  // Color Attachment 4 Emissive
    };
    static inline const vk::Format sDepthStencilFormat{vk::Format::eD32SfloatS8Uint};
    vk::DescriptorPool mDescriptorPool{};
    static inline const std::unordered_map<std::string, std::vector<vk::DescriptorSetLayoutBinding>>
        sDefaultDescriptorSetLayoutBindings{
            {DefaultDescriptorSetLayoutType::Bindless, // Per Frame
             {
                 // binding 0: Texture2D array
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(0)
                     .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                     .setDescriptorCount(MAX_DESCRIPTOR_COUNT) // Texture数组
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
             }},
            {DefaultDescriptorSetLayoutType::Global, // Per Frame
             {
                 // binding 0: VP
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(0)
                     .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                     .setDescriptorCount(1)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
                 // binding 1: Lights Storage Buffer
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(1)
                     .setDescriptorType(vk::DescriptorType::eStorageBuffer)
                     .setDescriptorCount(1)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
             }},
            {DefaultDescriptorSetLayoutType::Material, // Per Render Object
             {
                 // binding 0
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(0)
                     .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                     .setDescriptorCount(1)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
             }}};
    static inline const std::unordered_map<std::string, vk::PushConstantRange> sDefaultPushConstantRanges{
        {DefaultPushConstantRangeType::Matrix,
         vk::PushConstantRange{}
             .setOffset(0)
             .setSize(sizeof(glm::mat4))
             .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)},
    };
    std::unordered_map<std::string, vk::DescriptorSetLayout> mDefaultDescriptorSetLayouts{};
    std::unordered_map<std::string, vk::PushConstantRange> mDefaultPushConstantRanges{};

  public:
    std::shared_ptr<GraphicPipeline> GetGraphicPipeline(const Core::UUID &id);
    std::shared_ptr<GraphicPipeline> GetGraphicPipelineByName(const std::string &name);
    PipelineManager(std::shared_ptr<Context> context, std::shared_ptr<ShaderManager> shaderManager);
    ~PipelineManager() override;
    void Import(const AssetURL &url) override;
    void Export(std::shared_ptr<Asset> asset, const AssetURL &url) override;
    std::shared_ptr<Asset> Load(const AssetURL &url) override;
    void Save(std::shared_ptr<Asset> asset, const AssetURL &url) override;
};
} // namespace MEngine::Resource