#pragma once
#include "IUUIDGenerator.hpp"
#include "UUID.hpp"

namespace MEngine::Core
{
class UUIDv7Generator : public IUUIDGenerator
{
  public:
    UUID Create() override;
};
} // namespace MEngine::Core