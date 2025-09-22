#pragma once
#include "RHIBuffer.hpp"
#include "RHIHandler.hpp"
#include "RenderResource.hpp"
#include "Vertex.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class IndexResource final : public RenderResource
{
  protected:
    RHIHandler<RHIBuffer> mRHIVertexBufferHandler;

  public:
    IndexResource() : RenderResource()
    {
    }
    ~IndexResource() override = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    void UpdateData(const std::vector<uint32_t> &indices);
    inline RHIHandler<RHIBuffer> GetIndexBuffer() const
    {
        return mRHIVertexBufferHandler;
    }
};
} // namespace MEngine::Resource
