#pragma once
#include "Context.hpp"
#include "RenderResource.hpp"
#include "Vertex.hpp"
#include <cstdint>
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
    inline vk::Buffer GetVertexBuffer() const
    {
        return mVertexBuffer;
    }
    inline vk::Buffer GetIndexBuffer() const
    {
        return mIndexBuffer;
    }
    uint32_t GetIndexCount() const;
};
} // namespace MEngine::Resource