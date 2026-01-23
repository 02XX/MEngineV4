#pragma once
#include "Math.hpp"
#include "System.hpp"
#include "TransformComponent.hpp"
#include <memory>

namespace MEngine::Function
{
class TransformSystem final : public System
{
  private:
  public:
    TransformSystem(std::shared_ptr<Scene> scene) : System(scene)
    {
    }
    void Init() override;
    void Update(double deltaTime) override;
    void Shutdown() override;

  private:
    void CalculateMatrix(entt::entity entity);
};
} // namespace MEngine::Function