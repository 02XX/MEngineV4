#pragma once
#include "Component.hpp"
#include "Math.hpp"
namespace MEngine::Resource
{

enum class LightType : uint32_t
{
    Directional = 0,
    Point = 1,
    Spot = 2,
};

class LightComponent final : public Component
{
  public:
    ~LightComponent() override = default;
    LightType LightType = LightType::Directional;
    float Intensity = 1.0f;
    Vector3 Color = Vector3(1.0f);
    // point
    float Radius = 10.0f;
    // spot
    float InnerConeAngle = 0.0f; // in radians
    float OuterConeAngle = 0.0f; // in radians
};
struct LightParameters
{
    // base
    LightType LightType = LightType::Directional;

    float Intensity = 1.0f;

    // point
    float Radius = 10.0f;

    // spot
    float InnerConeAngle = 0.0f; // in radians
    float OuterConeAngle = 0.0f; // in radians
    int enable = 0;              // 是否启用光源，1表示启用，0表示禁用
    alignas(16) Vector3 Color = Vector3(1.0f);

    alignas(16) Vector3 Position = Vector3(0.0f);

    alignas(16) Vector3 Direction = Vector3(0.0f, 0.0f, 1.0f);
};
} // namespace MEngine::Resource