#pragma once
namespace MEngine::Resource
{
class Component
{
  public:
    virtual ~Component() = default;
    bool Enabled = true;
    bool Dirty = true;
};
} // namespace MEngine::Resource
