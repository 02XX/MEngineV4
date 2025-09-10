#pragma once
#include "Asset.hpp"
#include "PipelineLayoutType.hpp"
#include "UUID.hpp"
#include <vulkan/vulkan.hpp>
using namespace MEngine::Core;
namespace MEngine::Function
{
class PipelineLayoutBuilder;
class GBufferPBRPipelineLayoutBuilder;
class LightingPBRPipelineLayoutBuilder;
} // namespace MEngine::Function
namespace MEngine::Resource
{
class PipelineLayout : public Asset
{
    friend class Function::PipelineLayoutBuilder;
    friend class Function::GBufferPBRPipelineLayoutBuilder;
    friend class Function::LightingPBRPipelineLayoutBuilder;

  private:
    vk::UniquePipelineLayout mPipelineLayout{nullptr};

  protected:
    PipelineLayoutType mPipelineLayoutType{PipelineLayoutType::None};
    vk::PipelineLayoutCreateInfo mPipelineLayoutCreateInfo{};
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> mPipelineLayoutBindings{};
    std::vector<vk::UniqueDescriptorSetLayout> mSetLayouts{};
    std::vector<vk::PushConstantRange> mPushConstantRanges{};

  protected:
    PipelineLayout() : Asset()
    {
    }

  public:
    ~PipelineLayout() override = default;
    inline vk::PipelineLayout GetPipelineLayout() const
    {
        return mPipelineLayout.get();
    }
    inline PipelineLayoutType GetPipelineLayoutType() const
    {
        return mPipelineLayoutType;
    }
};
} // namespace MEngine::Resource