#include "PBRMaterialResource.hpp"
#include "Logger.hpp"
#include "PBRMaterial.hpp"
#include <cstring>
namespace MEngine::Resource
{
void PBRMaterialResource::InitRHI(std::shared_ptr<Context> context)
{
    auto pbrMaterial = static_cast<PBRMaterial *>(mOwnerAsset);
    pbrMaterial->mTextures.Albedo->GetResource()->InitResource(context);
    pbrMaterial->mTextures.Normal->GetResource()->InitResource(context);
    pbrMaterial->mTextures.ARM->GetResource()->InitResource(context);
    pbrMaterial->mTextures.Emissive->GetResource()->InitResource(context);
}
void PBRMaterialResource::ReleaseRHI(std::shared_ptr<Context> context)
{
}

} // namespace MEngine::Resource