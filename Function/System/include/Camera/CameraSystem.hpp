#pragma once
#include "System.hpp"
#include <memory>

namespace MEngine::Function
{
class CameraSystem final : public System
{
  private:
  public:
    CameraSystem(std::shared_ptr<Scene> scene) : System(scene)
    {
    }
    void Init() override;
    void Update(double deltaTime) override;
    void Shutdown() override;
};
} // namespace MEngine::Function