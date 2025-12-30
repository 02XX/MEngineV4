#pragma once
#include "RHIResource.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
struct RHIDescriptorSetDesc
{
    std::vector<vk::DescriptorSetLayout> SetLayouts{};
};
class RHIDescriptorSet : public RHIResource
{
  private:
    std::vector<vk::UniqueDescriptorSet> mDescriptorSets{};
    RHIDescriptorSetDesc mDescriptorSetDesc{};

  public:
    RHIDescriptorSet(const RHIDescriptorSetDesc &desc);
    virtual ~RHIDescriptorSet() override = default;
    inline std::vector<vk::DescriptorSet> GetDescriptorSets() const
    {
        std::vector<vk::DescriptorSet> descriptorSets;
        descriptorSets.reserve(mDescriptorSets.size());
        for (auto &set : mDescriptorSets)
        {
            descriptorSets.push_back(set.get());
        }
        return descriptorSets;
    }
};

} // namespace MEngine::Platform