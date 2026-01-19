#pragma once
#include "Component.hpp"
#include "Math.hpp"
#include <memory>

namespace MEngine::Resource
{
class CameraComponent : public Component
{
  public:
    ~CameraComponent() override = default;
    Vector3 target = {0.0f, 0.0f, 0.0f};
    Vector3 up = {0.0f, 1.0f, 0.0f};
    float fovY = 60;
    float zoom = 1.0f;
    float fovX = 60;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float aspectRatio = 16.0f / 9.0f;

    bool isMainCamera = false;
    bool isEditorCamera = false;
    Matrix4 viewMatrix = Matrix4(1.0f);
    Matrix4 projectionMatrix = Matrix4(1.0f);
};
struct CameraParameters
{
    alignas(16) Vector3 Position = Vector3(0.0f);
    alignas(16) Vector3 Direction = Vector3(0.0f, 0.0f, -1.0f);
    alignas(16) Matrix4 ProjectionMatrix = Matrix4(1.0f);
    alignas(16) Matrix4 ViewMatrix = Matrix4(1.0f);
};
} // namespace MEngine::Resource