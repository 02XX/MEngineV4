#pragma once
#include "Asset.hpp"
#include "Math.hpp"
#include "StaticMeshResource.hpp"
#include "Vertex.hpp"
#include <memory>
using namespace MEngine::Core;
namespace MEngine::Resource
{

class StaticMesh final : public Asset
{
  protected:
    Matrix4 mModelMatrix{};
    std::vector<Vertex> mVertices{};
    std::vector<uint32_t> mIndices{};

  public:
    StaticMesh(const std::string &name, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
        : Asset(name), mVertices(std::move(vertices)), mIndices(std::move(indices))
    {
        mResource = std::make_unique<StaticMeshResource>(&mVertices, &mIndices);
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