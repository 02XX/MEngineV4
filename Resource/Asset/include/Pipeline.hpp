#pragma once
#include "Asset.hpp"
#include <cstdint>
#include <string>
#include <vector>
using namespace MEngine::Core;
namespace MEngine::Resource
{

class Pipeline : public Asset
{
    friend class PipelineResource;

  public:
    std::vector<std::string> mDescriptorSetLayouts{};
    std::vector<std::string> mPushConstantRanges{};

  public:
    Pipeline(const std::string &name, std::vector<std::string> descriptorSetLayouts,
             std::vector<std::string> pushConstantRanges)
        : Asset(name), mDescriptorSetLayouts(descriptorSetLayouts), mPushConstantRanges(pushConstantRanges)
    {
        mAssetType = AssetType::Pipeline;
    }
};
} // namespace MEngine::Resource