#pragma once
#include "ISystem.hpp"
#include <memory>
namespace MEngine::Function
{
class System : public ISystem
{
  protected:
  public:
    System()

    {
    }
    virtual ~System() = default;
};

} // namespace MEngine::Function