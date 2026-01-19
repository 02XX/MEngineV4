#pragma once
#include "AssetURL.hpp"
#include "Manager.hpp"
#include "MeshManager.hpp"
#include "PBRMaterialManager.hpp"
#include "Scene.hpp"
#include "UpdateManager.hpp"
#include <memory>

namespace MEngine::Resource
{
struct DefaultScene
{
    static constexpr const char *Sample = "Sample";
};
class SceneManager final : public UpdateManager<Scene>, public virtual IManager<Scene>
{
  private:
    std::shared_ptr<PBRMaterialManager> mPBRMaterialManager;
    std::shared_ptr<MeshManager> mMeshManager;

  private:
    static inline const std::unordered_map<std::string, Core::UUID> mDefaultScenes{
        {DefaultScene::Sample, Core::UUID{"20000000-0000-0000-0000-000000000000"}},
    };

  public:
    SceneManager(std::shared_ptr<PBRMaterialManager> pbrMaterialManager, std::shared_ptr<MeshManager> meshManager)
        : UpdateManager<Scene>(), mPBRMaterialManager(pbrMaterialManager), mMeshManager(meshManager)
    {
        CreateDefault();
    }
    ~SceneManager() override = default;
    void CreateDefault() override;
    std::shared_ptr<Scene> CreateSample();
    void UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                   vk::CommandBufferInheritanceInfo *inheritanceInfo = nullptr) override;

  private:
};
} // namespace MEngine::Resource