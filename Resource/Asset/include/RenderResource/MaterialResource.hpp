#pragma once
#include "Context.hpp"
#include "IBind.hpp"
#include "IUpload.hpp"
#include "RenderResource.hpp"
#include <array>
#include <cstdint>
#include <memory>

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Material;
// Abstract class
class MaterialResource : public RenderResource, public virtual IBind
{
  public:
  public:
    MaterialResource(Material *material);
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    void Bind(BindContext bindContext) override;
};
} // namespace MEngine::Resource