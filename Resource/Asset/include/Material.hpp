#pragma once
#include "Asset.hpp"
#include "GraphicPipeline.hpp"
#include "MaterialResource.hpp"
#include <memory>
#include <string>

namespace MEngine::Resource
{
// Abstract class
class Material : public Asset
{
    friend class MaterialResource;

  protected:
    std::shared_ptr<GraphicPipeline> mPipeline{};

  public:
    Material(const std::string &name, const std::shared_ptr<GraphicPipeline> &pipeline)
        : Asset(name), mPipeline(pipeline)
    {
    }
    ~Material() override = default;
    inline std::shared_ptr<GraphicPipeline> GetPipeline() const
    {
        return mPipeline;
    }
};
} // namespace MEngine::Resource