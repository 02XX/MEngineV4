#pragma once
#include "Asset.hpp"
#include "ECS.hpp"
#include "SceneResource.hpp"
#include <memory>
namespace MEngine::Resource
{
inline constexpr uint32_t MAX_LIGHTS = 16;
struct LightParam
{
    glm::vec4 Color;
    glm::vec4 Position;
    glm::vec4 Direction;

    uint32_t LightType;
    uint32_t Enable;
    float Intensity;
    float Radius;

    float InnerConeAngle;
    float OuterConeAngle;
};
struct SceneParam
{
    Matrix4 ViewMatrix;
    Matrix4 ProjectionMatrix;
    Vector4 CameraPosition;
};
class Scene : public Asset
{
  public:
    std::shared_ptr<Register> mRegistry;
    SceneParam mSceneParams{};
    bool mSceneParamsDirty = true;
    LightParam mLightParams[MAX_LIGHTS]{};
    bool mLightParamsDirty = true;

  public:
    Scene(const std::string &name);
    ~Scene() override = default;
};
} // namespace MEngine::Resource