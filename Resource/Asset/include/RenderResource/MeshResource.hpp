#pragma once
#include "Context.hpp"
#include "IBind.hpp"
#include "IUpload.hpp"
#include "RenderResource.hpp"
#include "Vertex.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace MEngine::Resource
{
class Mesh;
class MeshResource final : public RenderResource, public virtual IUpload, public virtual IBind
{
  public:
    // Vertex
    vk::Buffer mVertexBuffer{}, mStagingVertexBuffer{};
    VmaAllocation mVertexBufferAllocation{}, mStagingVertexBufferAllocation{};
    VmaAllocationInfo mVertexBufferAllocationInfo{}, mStagingVertexBufferAllocationInfo{};
    // Index
    vk::Buffer mIndexBuffer{}, mStagingIndexBuffer{};
    VmaAllocation mIndexBufferAllocation{}, mStagingIndexBufferAllocation{};
    VmaAllocationInfo mIndexBufferAllocationInfo{}, mStagingIndexBufferAllocationInfo{};

  public:
    MeshResource(Mesh *staticMesh);

    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    void InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize) override;
    void ReleaseStaging(std::shared_ptr<Context> context) override;
    void UploadData() override;
    void Bind(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout = {},
              vk::Pipeline pipeline = {}) override;
};
} // namespace MEngine::Resource