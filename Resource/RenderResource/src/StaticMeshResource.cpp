#include "StaticMeshResource.hpp"
#include "StaticMesh.hpp"
namespace MEngine::Resource
{
void StaticMeshResource::InitRHI()
{
    mVertexResource = std::make_unique<VertexResource>();
    mIndexResource = std::make_unique<IndexResource>();
    mVertexResource->InitRHI();
    mVertexResource->UpdateData(mStaticMesh->mVertices);
    mIndexResource->InitRHI();
    mIndexResource->UpdateData(mStaticMesh->mIndices);
}
void StaticMeshResource::ReleaseRHI()
{
    if (mVertexResource)
        mVertexResource->ReleaseRHI();
    if (mIndexResource)
        mIndexResource->ReleaseRHI();
}

} // namespace MEngine::Resource