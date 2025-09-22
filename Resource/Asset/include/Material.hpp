#pragma once
#include "Asset.hpp"
#include "GraphicPipeline.hpp"
#include "MaterialResource.hpp"
#include <memory>
#include <string>

namespace MEngine::Resource
{
class Material : public Asset
{
    friend class MaterialResource;

  protected:
    std::shared_ptr<GraphicPipeline> mPipeline{};

  protected:
    Material() : Asset()
    {
    }

  public:
    Material(const std::string &name, const std::shared_ptr<GraphicPipeline> &pipeline)
        : Asset(name), mPipeline(pipeline)
    {
        mResource = std::make_unique<MaterialResource>(this);
    }
    virtual ~Material() = default;
    inline const std::shared_ptr<const GraphicPipeline> GetPipeline() const
    {
        return mPipeline;
    }
};
} // namespace MEngine::Resource