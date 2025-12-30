#pragma once
#include "Asset.hpp"
#include "CameraComponent.hpp"
#include "ECS.hpp"
#include "SceneResource.hpp"
#include "TransformComponent.hpp"
#include <memory>

namespace MEngine::Resource
{
class Scene : public Asset
{
  private:
    std::shared_ptr<Register> mRegistry;
    Entity mMainCameraEntity = NullEntity;

  protected:
    Scene() : Asset()
    {
       mName = "Scene";
        mRegistry = std::make_shared<Register>();
    }

  public:
    Scene(const std::string &name) : Asset(name)
    {
        mResource = std::make_unique<SceneResource>(this);
        mRegistry = std::make_shared<Register>();
        mMainCameraEntity = mRegistry->create();
        TransformComponent transformComponent;
        CameraComponent cameraComponent;
        cameraComponent.isMainCamera = true;
        mRegistry->emplace<TransformComponent>(mMainCameraEntity, transformComponent);
        mRegistry->emplace<CameraComponent>(mMainCameraEntity, cameraComponent);
    }
    virtual ~Scene() = default;
    inline std::shared_ptr<Register> GetRegistry() const
    {
        return mRegistry;
    }
    inline Entity GetMainCameraEntity() const
    {
        return mMainCameraEntity;
    }
};
} // namespace MEngine::Resource