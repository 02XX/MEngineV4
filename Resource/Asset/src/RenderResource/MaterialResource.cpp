#include "MaterialResource.hpp"
#include "Material.hpp"
namespace MEngine::Resource
{
MaterialResource::MaterialResource(Material *material) : RenderResource(material)
{
}
} // namespace MEngine::Resource