#pragma once
#include "Asset.hpp"
#include <vector>
using namespace MEngine::Core;
namespace MEngine::Resource
{
class Pipeline : public Asset
{
    friend class PipelineResource;

  protected:
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> mDescriptorSetLayoutBindings{};
    std::vector<vk::PushConstantRange> mPushConstantRanges{};

  public:
    Pipeline(const std::string &name,
             std::vector<std::vector<vk::DescriptorSetLayoutBinding>> descriptorSetLayoutBindings,
             std::vector<vk::PushConstantRange> pushConstantRanges)
        : Asset(name), mDescriptorSetLayoutBindings(descriptorSetLayoutBindings),
          mPushConstantRanges(pushConstantRanges)
    {
        
    }
    ~Pipeline() override = default;
};
} // namespace MEngine::Resource