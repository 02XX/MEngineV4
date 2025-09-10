#pragma once

namespace MEngine::Function
{

class ISystem
{
  public:
    virtual ~ISystem() = default;
    virtual void Init() = 0;
    virtual void Update(double deltaTime) = 0;
    virtual void Shutdown() = 0;
};

} // namespace MEngine::Function