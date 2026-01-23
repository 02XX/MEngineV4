#pragma once
#include "IPendingResourceManager.hpp"
#include "Manager.hpp"
#include "Mesh.hpp"
#include "MeshResource.hpp"
#include "PendingResourceManager.hpp"
#include <cstdint>
#include <memory>

namespace MEngine::Resource
{
struct DefaultMeshType
{
    static inline const char *Triangle = "Triangle";
    static inline const char *Cube = "Cube";
    static inline const char *Sphere = "Sphere";
    static inline const char *Plane = "Plane";
    static inline const char *Cylinder = "Cylinder";
    static inline const char *Sky = "Sky";
    static inline const char *FullscreenTriangle = "FullscreenTriangle";
};
class MeshManager final : public Manager<Mesh, MeshResource>
{
  private:
    std::unordered_map<std::string, Core::UUID> mDefaultMeshes{
        {DefaultMeshType::Triangle, Core::UUID{"00000000-0000-0000-0000-000000000001"}},
        {DefaultMeshType::Cube, Core::UUID{"00000000-0000-0000-0000-000000000002"}},
        {DefaultMeshType::Sphere, Core::UUID{"00000000-0000-0000-0000-000000000003"}},
        {DefaultMeshType::Plane, Core::UUID{"00000000-0000-0000-0000-000000000004"}},
        {DefaultMeshType::Cylinder, Core::UUID{"00000000-0000-0000-0000-000000000005"}},
        {DefaultMeshType::Sky, Core::UUID{"00000000-0000-0000-0000-000000000006"}},
        {DefaultMeshType::FullscreenTriangle, Core::UUID{"00000000-0000-0000-0000-000000000007"}},
    };

  private:
    std::shared_ptr<Mesh> CreateTriangle();
    std::shared_ptr<Mesh> CreateCube();
    std::shared_ptr<Mesh> CreateSphere();
    std::shared_ptr<Mesh> CreatePlane();
    std::shared_ptr<Mesh> CreateCylinder();
    std::shared_ptr<Mesh> CreateSky();
    std::shared_ptr<Mesh> CreateFullscreenTriangle();

  public:
    MeshManager(std::shared_ptr<Context> context);
    ~MeshManager() override = default;
    std::shared_ptr<Asset> Load(const AssetURL &url) override
    {
    }
    void Save(std::shared_ptr<Asset> asset, const AssetURL &url) override
    {
    }
    void ProcessPendingUpdateResources(RenderContext renderContext) override;
};

} // namespace MEngine::Resource