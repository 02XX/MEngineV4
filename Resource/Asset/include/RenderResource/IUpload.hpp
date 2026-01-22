#pragma once
#include "Context.hpp"
#include <memory>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class IUpload
{
  public:
    virtual ~IUpload() = default;
    virtual void InitStaging(std::shared_ptr<Context> context, vk::DeviceSize bufferSize) = 0;
    virtual void ReleaseStaging(std::shared_ptr<Context> context) = 0;
    virtual void UploadData() = 0;
};
} // namespace MEngine::Resource