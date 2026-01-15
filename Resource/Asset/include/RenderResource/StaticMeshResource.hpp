#pragma once
#include "Context.hpp"
#include "RenderResource.hpp"
#include "Vertex.hpp"
#include <vector>
namespace MEngine::Resource
{
class StaticMesh;
class StaticMeshResource : public RenderResource
{
  protected:
    // Vulkan Resource
    // Vertex
    vk::Buffer mVertexBuffer;
    VmaAllocation mVertexBufferAllocation;
    VmaAllocationInfo mVertexBufferAllocationInfo;
    // Index
    vk::Buffer mIndexBuffer;
    VmaAllocation mIndexBufferAllocation;
    VmaAllocationInfo mIndexBufferAllocationInfo;

  public:
    StaticMeshResource(StaticMesh *staticMesh);
    ~StaticMeshResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource