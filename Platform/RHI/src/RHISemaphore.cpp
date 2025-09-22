#include "RHISampler.hpp"
#include "RHIContext.hpp"
namespace MEngine::Platform
{
RHISampler::RHISampler(const RHISamplerDesc &desc) : RHIResource(), mSamplerDesc(desc)
{
    auto &rhiContext = RHIContext::Instance();
    auto &device = rhiContext.GetDevice();
    mSampler = device.createSamplerUnique(mSamplerDesc);
    if (!mSampler)
    {
        throw std::runtime_error("Failed to create sampler!");
    }
}
} // namespace MEngine::Platform