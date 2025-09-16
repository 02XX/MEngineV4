#pragma once
#include "Asset.hpp"
#include "StaticMeshResource.hpp"
#include "Vertex.hpp"
#include "vk_mem_alloc.h"
#include <memory>
#include <nlohmann/json.hpp>

namespace MEngine::Resource
{

class StaticMesh final : public Asset
{
    friend class StaticMeshResource;

  protected:
    glm::mat4 mModelMatrix{};
    std::vector<Vertex> mVertices{};
    std::vector<uint32_t> mIndices{};

    std::unique_ptr<StaticMeshResource> mResource;

  public:
    StaticMesh() : Asset()
    {
        mResource = std::make_unique<StaticMeshResource>(this);
    }
    ~StaticMesh() override = default;
    inline const std::vector<Vertex> &GetVertices() const
    {
        return mVertices;
    }
    inline const std::vector<uint32_t> &GetIndices() const
    {
        return mIndices;
    }
};
} // namespace MEngine::Resource