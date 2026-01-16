
#pragma once
#include "TextureResource.hpp"
#include <cstdint>

using namespace MEngine::Platform;
namespace MEngine::Resource
{

class Texture2D;
class Texture2DResource : public TextureResource
{

  public:
    uint32_t mBindlessDescriptorIndex{0};

  public:
    Texture2DResource(Texture2D *texture2D);
    ~Texture2DResource() override = default;
    virtual void InitRHI(std::shared_ptr<Context> context) override;
    virtual void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource
