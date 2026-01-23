#pragma once
#include "Context.hpp"
#include "IBind.hpp"
#include "PendingResourceManager.hpp"
#include "PipelineManager.hpp"
#include "Texture.hpp"
#include "Texture2D.hpp"
#include "Texture2DArray.hpp"
#include "TextureCube.hpp"
#include "TextureCubeArray.hpp"
#include "UUID.hpp"
#include <cstdint>
#include <memory>
#include <queue>
#include <set>
#include <typeindex>
#include <unordered_map>

namespace MEngine::Resource
{
struct DefaultTextureType
{
    static constexpr const char *Magenta2D = "Magenta2D";
    static constexpr const char *White2D = "White2D";
    static constexpr const char *Black2D = "Black2D";
    static constexpr const char *Magenta2DArray = "Magenta2DArray";
    static constexpr const char *White2DArray = "White2DArray";
    static constexpr const char *Black2DArray = "Black2DArray";
    static constexpr const char *MagentaCube = "MagentaCube";
    static constexpr const char *WhiteCube = "WhiteCube";
    static constexpr const char *BlackCube = "BlackCube";
    static constexpr const char *MagentaCubeArray = "MagentaCubeArray";
    static constexpr const char *WhiteCubeArray = "WhiteCubeArray";
    static constexpr const char *BlackCubeArray = "BlackCubeArray";
};
class TextureManager final : public virtual Manager<Texture, TextureResource>, public virtual IBind
{
  private:
    std::shared_ptr<PipelineManager> mPipelineManager{};
    vk::BindDescriptorSetsInfo mBindInfo{};

  private:
    static inline const std::unordered_map<std::string, Core::UUID> sDefaultTextures{
        {DefaultTextureType::Magenta2D, Core::UUID{"10000000-0000-0000-0000-000000000000"}},
        {DefaultTextureType::White2D, Core::UUID{"10000000-0000-0000-0000-000000000001"}},
        {DefaultTextureType::Black2D, Core::UUID{"10000000-0000-0000-0000-000000000002"}},
        {DefaultTextureType::Magenta2DArray, Core::UUID{"10000000-0000-0000-0000-000000000003"}},
        {DefaultTextureType::White2DArray, Core::UUID{"10000000-0000-0000-0000-000000000004"}},
        {DefaultTextureType::Black2DArray, Core::UUID{"10000000-0000-0000-0000-000000000005"}},
        {DefaultTextureType::MagentaCube, Core::UUID{"10000000-0000-0000-0000-000000000006"}},
        {DefaultTextureType::WhiteCube, Core::UUID{"10000000-0000-0000-0000-000000000007"}},
        {DefaultTextureType::BlackCube, Core::UUID{"10000000-0000-0000-0000-000000000008"}},
        {DefaultTextureType::MagentaCubeArray, Core::UUID{"10000000-0000-0000-0000-000000000009"}},
        {DefaultTextureType::WhiteCubeArray, Core::UUID{"10000000-0000-0000-0000-00000000000A"}},
        {DefaultTextureType::BlackCubeArray, Core::UUID{"10000000-0000-0000-0000-00000000000B"}},
    };

  public:
    vk::DescriptorSet mTextureBindlessDescriptorSet{};

  private:
    std::queue<uint32_t> mFreeDescriptorIndices{};
    uint32_t mNextDescriptorIndex{0};
    std::unordered_map<Core::UUID, uint32_t> mTextureDescriptorIndices{};

  public:
    TextureManager(std::shared_ptr<Context> context, std::shared_ptr<PipelineManager> pipelineManager);
    ~TextureManager() override;
    std::shared_ptr<Asset> Load(const AssetURL &url) override
    {
    }
    void Save(std::shared_ptr<Asset> asset, const AssetURL &url) override
    {
    }
    uint32_t AllocateDescriptorIndex();
    void FreeDescriptorIndex(uint32_t index);
    void Bind(BindContext bindContext) override;
    void ProcessPendingUpdateResources(RenderContext renderContext) override;
    void ProcessPendingDeletionResources(RenderContext renderContext) override;
};

} // namespace MEngine::Resource