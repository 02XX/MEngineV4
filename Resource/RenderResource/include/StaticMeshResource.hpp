#pragma once
#include "Context.hpp"
#include "RenderResource.hpp"
#include "Vertex.hpp"
#include <vector>
namespace MEngine::Resource
{
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
    // Data 所有权在上层
    const std::vector<Vertex> *mVertices{};
    const std::vector<uint32_t> *mIndices{};

  public:
    StaticMeshResource(const std::vector<Vertex> *vertices, const std::vector<uint32_t> *indices)
        : RenderResource(), mVertices(vertices), mIndices(indices)
    {
    }
    ~StaticMeshResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource