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

  public:
    std::shared_ptr<GraphicPipeline> mPipeline{};
    bool mDynamic{false};

  protected:
    Material() : Asset()
    {
        mAssetType = AssetType::Material;
    }

  public:
    Material(const std::string &name, std::shared_ptr<GraphicPipeline> pipeline, bool dynamic = false)
        : Asset(name), mPipeline(pipeline), mDynamic(dynamic)
    {
    }
};
} // namespace MEngine::Resource