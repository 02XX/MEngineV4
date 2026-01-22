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
    LightType LightType = LightType::Directional;
    float Intensity = 1.0f;
    Vector3 Color = Vector3(1.0f);
    // point
    float Radius = 10.0f;
    // spot
    float InnerConeAngle = 0.0f; // in radians
    float OuterConeAngle = 0.0f; // in radians
};

} // namespace MEngine::Resource