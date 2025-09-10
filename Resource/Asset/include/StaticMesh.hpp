#pragma once
#include "Asset.hpp"
#include "Vertex.hpp"
#include "vk_mem_alloc.h"
#include <string>

namespace MEngine::Resource
{
class StaticMesh : public Asset
{
  protected:
    glm::mat4 mModelMatrix{};
    std::vector<Vertex> mVertices{};
    std::vector<uint32_t> mIndices{};

    vk::Buffer mVertexBuffer{};
    vk::Buffer mIndexBuffer{};
    VmaAllocation mVertexBufferAllocation{};
    VmaAllocation mIndexBufferAllocation{};
    VmaAllocationInfo mVertexBufferAllocationInfo{};
    VmaAllocationInfo mIndexBufferAllocationInfo{};

  protected:
    StaticMesh() : Asset()
    {
    }

  public:
    virtual ~StaticMesh() = default;
    inline const std::vector<Vertex> &GetVertices() const
    {
        return mVertices;
    }
    inline const std::vector<uint32_t> &GetIndices() const
    {
        return mIndices;
    }
    inline const vk::Buffer GetVertexBuffer() const
    {
        return mVertexBuffer;
    }
    inline const vk::Buffer GetIndexBuffer() const
    {
        return mIndexBuffer;
    }
};
} // namespace MEngine::Resource