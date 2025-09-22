#pragma once
#include "Asset.hpp"
#include "Pipeline.hpp"
#include "UUID.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>
using namespace MEngine::Core;
namespace MEngine::Resource
{
struct PipelineLayoutDesc
{
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> DescriptorSetLayoutBindings{};
    std::vector<vk::PushConstantRange> PushConstantRanges{};
    vk::PipelineLayoutCreateFlags Flags{};
};
class Pipeline : public Asset
{
  protected:
    PipelineLayoutDesc mPipelineLayoutDesc{};

  protected:
    Pipeline() : Asset()
    {
    }

  public:
    Pipeline(const std::string &name, const PipelineLayoutDesc &layoutDesc)
        : Asset(name), mPipelineLayoutDesc(layoutDesc)
    {
    }
    ~Pipeline() override = default;
};
} // namespace MEngine::Resource