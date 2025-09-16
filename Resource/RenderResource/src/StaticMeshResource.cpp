#include "StaticMeshResource.hpp"
#include "StaticMesh.hpp"
namespace MEngine::Resource
{
void StaticMeshResource::InitRHI()
{
    mVertexResource = std::make_unique<VertexResource>();
    mVertexResource->InitRHI();
    mVertexResource->UpdateData(mStaticMesh->mVertices);

    mIndexResource = std::make_unique<IndexResource>();
    mIndexResource->InitRHI();
    mIndexResource->UpdateData(mStaticMesh->mIndices);
}
void StaticMeshResource::ReleaseRHI()
{
    mVertexResource->ReleaseRHI();
    mIndexResource->ReleaseRHI();
}

} // namespace MEngine::Resource