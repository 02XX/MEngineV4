
#include "Scene.hpp"

namespace MEngine::Resource
{
Scene::Scene(const std::string &name) : Asset(name)
{
    mRegistry = std::make_shared<Register>();
    mResource = std::make_unique<SceneResource>(this);
}
} // namespace MEngine::Resource