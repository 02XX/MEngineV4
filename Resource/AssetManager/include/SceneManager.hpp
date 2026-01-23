#pragma once
#include "AssetURL.hpp"
#include "Manager.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "PendingResourceManager.hpp"
#include "PipelineManager.hpp"
#include "Scene.hpp"
#include "SceneResource.hpp"
#include <memory>

namespace MEngine::Resource
{
struct DefaultScene
{
    static constexpr const char *Sample = "Sample";
};
class SceneManager final : public Manager<Scene, SceneResource>
{
  private:
    std::shared_ptr<PipelineManager> mPipelineManager{};
    std::shared_ptr<MaterialManager> mMaterialManager{};
    std::shared_ptr<MeshManager> mMeshManager{};

  private:
    static inline const std::unordered_map<std::string, Core::UUID> mDefaultScenes{
        {DefaultScene::Sample, Core::UUID{"20000000-0000-0000-0000-000000000000"}},
    };

  public:
    SceneManager(std::shared_ptr<Context> context, std::shared_ptr<PipelineManager> pipelineManager,
                 std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<MeshManager> meshManager);
    ~SceneManager() override = default;
    std::shared_ptr<Asset> Load(const AssetURL &url) override
    {
    }
    void Save(std::shared_ptr<Asset> asset, const AssetURL &url) override
    {
    }
    std::shared_ptr<Scene> CreateSample();
    void ProcessPendingInitResources(RenderContext renderContext) override;
    void ProcessPendingUpdateResources(RenderContext renderContext) override;
};
} // namespace MEngine::Resource