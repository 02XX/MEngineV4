#pragma once
#include "Asset.hpp"
#include "Math.hpp"
#include "MeshResource.hpp"
#include "Vertex.hpp"
#include <memory>
using namespace MEngine::Core;
namespace MEngine::Resource
{

class Mesh final : public Asset
{
    friend class MeshResource;

  public:
    Matrix4 mModelMatrix{};
    std::vector<Vertex> mVertices{};
    std::vector<uint32_t> mIndices{};
    bool mDynamic{false};

  public:
    Mesh(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
         bool dynamic = false)
        : Asset(name), mVertices(vertices), mIndices(indices), mDynamic(dynamic)
    {
        mAssetType = AssetType::Mesh;
        mResource = std::make_unique<MeshResource>(this);
    }
};
} // namespace MEngine::Resource