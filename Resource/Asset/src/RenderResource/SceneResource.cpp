#include "SceneResource.hpp"
#include "Logger.hpp"
#include "Scene.hpp"
#include "VMA.hpp"
namespace MEngine::Resource
{
SceneResource::SceneResource(Scene *scene) : RenderResource(scene), mScene(scene)
{
}
void SceneResource::InitRHI(std::shared_ptr<Context> context)
{
}
void SceneResource::ReleaseRHI(std::shared_ptr<Context> context)
{
}
} // namespace MEngine::Resource
