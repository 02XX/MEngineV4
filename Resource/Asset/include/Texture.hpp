#pragma once
#include "Asset.hpp"
#include "UUID.hpp"
#include <filesystem>
#include <gtest/gtest_prod.h>
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
namespace MEngine::Resource
{
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/Engine/UTexture
class Texture : public Asset
{
  private:
  protected:
    std::vector<std::vector<uint8_t>> mMipDatas{};
    uint32_t mWidth{0};
    uint32_t mHeight{0};
    uint32_t mChannels{4};

  protected:
    Texture() : Asset()
    {
    }

  public:
    ~Texture() override = default;
    inline const std::vector<std::vector<uint8_t>> &GetMipDatas() const
    {
        return mMipDatas;
    }
    inline uint32_t GetWidth() const
    {
        return mWidth;
    }
    inline uint32_t GetHeight() const
    {
        return mHeight;
    }
    inline uint32_t GetChannels() const
    {
        return mChannels;
    }
};
} // namespace MEngine::Resource