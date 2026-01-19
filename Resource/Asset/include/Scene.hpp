#pragma once
#include "Asset.hpp"
#include "ECS.hpp"
#include "SceneResource.hpp"
#include <cstdint>
#include <memory>
namespace MEngine::Resource
{
inline constexpr uint32_t MAX_LIGHTS = 16;
struct LightParam
{
    glm::vec4 Color{1.0f};
    glm::vec4 Position{0.0f};
    glm::vec4 Direction{0.0f, 0.0f, -1.0f, 1.0f};

    uint32_t LightType{0};
    uint32_t Enable{1};
    float Intensity{1.0f};
    float Radius{0.0f};

    float InnerConeAngle{0.0f};
    float OuterConeAngle{0.0f};
};
struct SceneParam
{
    Matrix4 ViewMatrix{1.0f};
    Matrix4 ProjectionMatrix{1.0f};
    Vector4 CameraPosition{0.0f};
    Vector4 CameraDirection{0.0f, 0.0f, -1.0f, 1.0f};
    uint32_t LightCount{0};
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