#include "RHIResourceManager.hpp"

namespace MEngine::Platform
{
RHIResourceManager &RHIResourceManager::Instance()
{
    static RHIResourceManager instance;
    return instance;
}

} // namespace MEngine::Platform