#pragma once
#include "Component.hpp"
#include "Math.hpp"
namespace MEngine::Function
{
inline constexpr uint32_t MAX_LIGHTS = 16;
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
struct LightParam // aligned 16
{
    glm::vec4 Color;     // 0 aligned 16
    glm::vec4 Position;  // 16
    glm::vec4 Direction; // 32

    uint32_t LightType; // 48
    uint32_t Enable;    // 52
    float Intensity;    // 56
    float Radius;       // 60

    float InnerConeAngle;
    float OuterConeAngle;
};
} // namespace MEngine::Function