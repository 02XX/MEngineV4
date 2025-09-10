#pragma once
#include "Asset.hpp"
#include "GraphicPipeline.hpp"
#include <memory>
#include <string>
namespace MEngine::Resource
{
class Material : public Asset
{
  protected:
    std::string mPipelineName;
    vk::UniqueDescriptorSet mDescriptorSet;

  protected:
    Material() : Asset()
    {
    }

  public:
    virtual ~Material() = default;
    inline vk::DescriptorSet GetDescriptorSet() const
    {
        return mDescriptorSet.get();
    }
};
} // namespace MEngine::Resource