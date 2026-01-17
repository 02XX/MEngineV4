#pragma once
namespace MEngine::Function
{
class Component
{
  public:
    virtual ~Component() = default;
    bool dirty = false;
};
} // namespace MEngine::Function
