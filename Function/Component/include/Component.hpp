#pragma once
namespace MEngine::Function
{
class Component
{
  public:
    virtual ~Component() = default;
    bool Enabled = true;
    bool Dirty = true;
};
} // namespace MEngine::Function
