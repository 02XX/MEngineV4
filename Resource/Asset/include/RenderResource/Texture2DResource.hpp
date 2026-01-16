
#pragma once
#include "TextureResource.hpp"
#include <cstdint>

using namespace MEngine::Platform;
namespace MEngine::Resource
{

class Texture2D;
class Texture2DResource : public TextureResource
{

  protected:
    uint32_t mBindlessDescriptorIndex{0};

  public:
    Texture2DResource(Texture2D *texture2D);
    ~Texture2DResource() override = default;
    virtual void InitRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource
