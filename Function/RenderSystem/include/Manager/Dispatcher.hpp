

#include "Asset.hpp"
#include "IManager.hpp"
#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vcruntime_typeinfo.h>
namespace MEngine::Function
{
class Dispatcher
{
  private:
    std::unordered_map<std::type_index, std::shared_ptr<IManager<Asset>>> mManagers;

  public:
    template <std::derived_from<Asset> TAsset> void Register(std::shared_ptr<TAsset> manager)
    {
        mManagers[typeid(TAsset)] = manager;
    }
    template <std::derived_from<Asset> TAsset> void Update(std::shared_ptr<Asset> asset)
    {
        auto assetRef = *asset;
        auto type = std::type_index(typeid(assetRef));
        mManagers[type]->Update(asset->GetID(), asset);
    }
};
} // namespace MEngine::Function