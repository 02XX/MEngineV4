#pragma once
#include "Asset.hpp"
#include <cstdint>
#include <vector>
using namespace MEngine::Core;
namespace MEngine::Resource
{

class Pipeline : public Asset
{
    friend class PipelineResource;

  protected:
    std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts{};
    std::vector<vk::PushConstantRange> mPushConstantRanges{};

  public:
    Pipeline(const std::string &name, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts,
             std::vector<vk::PushConstantRange> pushConstantRanges)
        : Asset(name), mDescriptorSetLayouts(descriptorSetLayouts), mPushConstantRanges(pushConstantRanges)
    {
        mAssetType = AssetType::Pipeline;
    }
};
} // namespace MEngine::Resource