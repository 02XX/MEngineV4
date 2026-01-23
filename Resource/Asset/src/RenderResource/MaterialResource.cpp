#include "MaterialResource.hpp"
#include "AssetManager.hpp"
#include "Material.hpp"
#include "PipelineManager.hpp"
namespace MEngine::Resource
{
MaterialResource::MaterialResource(Material *material) : RenderResource(material)
{
}
void MaterialResource::InitRHI(std::shared_ptr<Context> context)
{
}
void MaterialResource::ReleaseRHI(std::shared_ptr<Context> context)
{
}

} // namespace MEngine::Resource