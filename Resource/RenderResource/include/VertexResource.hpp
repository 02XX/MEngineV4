#pragma once
#include "RHIBuffer.hpp"
#include "RHIHandler.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"
#include "RenderResource.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class VertexResource final : public RenderResource
{
  protected:
    RHIHandler<RHIBuffer> mRHIVertexBufferHandler;

  public:
    VertexResource() : RenderResource()
    {
    }
    ~VertexResource() override = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    void UpdateData(const std::vector<Vertex> &vertices);
};
} // namespace MEngine::Resource
