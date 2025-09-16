#pragma once
#include "IndexResource.hpp"
#include "VertexResource.hpp"

namespace MEngine::Resource
{
class StaticMesh;
class StaticMeshResource : public RenderResource
{
    friend class StaticMesh;

  protected:
    std::unique_ptr<VertexResource> mVertexResource{};
    std::unique_ptr<IndexResource> mIndexResource{};
    StaticMesh *mStaticMesh{nullptr};

  public:
    StaticMeshResource(StaticMesh *mesh) : RenderResource(), mStaticMesh(mesh)
    {
    }
    ~StaticMeshResource() override = default;
    void InitRHI() override;
    void ReleaseRHI() override;
};
} // namespace MEngine::Resource