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
    GraphicPipeline *mPipeline{};
    vk::DeviceSize mBufferSize{};
    bool mDynamic{false};

  protected:
    Material() : Asset()
    {
        mAssetType = AssetType::Material;
    }

  public:
    Material(const std::string &name, GraphicPipeline *pipeline, vk::DeviceSize bufferSize, bool dynamic = true)
        : Asset(name), mPipeline(pipeline), mDynamic(dynamic), mBufferSize(bufferSize)
    {
        mResource = std::make_unique<MaterialResource>(this);
    }
    virtual void UpdateMaterialData(uint8_t *target) = 0;
};
} // namespace MEngine::Resource