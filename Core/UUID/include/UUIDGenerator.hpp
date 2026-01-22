#pragma once
#include "IUUIDGenerator.hpp"
#include "UUIDv7Generator.hpp"
#include <memory>

namespace MEngine::Core
{
class UUIDGenerator final : public IUUIDGenerator
{
  private:
    std::unique_ptr<IUUIDGenerator> mUUIDGenerator;

  public:
    UUIDGenerator();

    static UUIDGenerator &Instance();
    UUID Create() override;
};
} // namespace MEngine::Core