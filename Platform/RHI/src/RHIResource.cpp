
#include "RHIResource.hpp"
#include "MPMCQueue.hpp"
#include "RHIHandler.hpp"
namespace MEngine::Platform
{
MPMCQueue<RHIResource *> PendingDeletes;
} // namespace MEngine::Platform
