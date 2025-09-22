#pragma once
#include "Manager.hpp"
#include "StaticMesh.hpp"
#include <cstdint>
#include <memory>
namespace MEngine::Resource
{
enum class DefaultMeshType
{
    Cube,
    Cylinder,
    Plane,
    Sphere,
    Sky,
    FullscreenTriangle
};
class MeshManager final : public Manager<StaticMesh>, public virtual IManager<StaticMesh>
{
  private:
    std::unordered_map<DefaultMeshType, UUID> mDefaultMeshes{
        {DefaultMeshType::Cube, UUID{"00000000-0000-0000-0000-000000000001"}},
        {DefaultMeshType::Cylinder, UUID{"00000000-0000-0000-0000-000000000002"}},
        {DefaultMeshType::Plane, UUID{"00000000-0000-0000-0000-000000000003"}},
        {DefaultMeshType::Sphere, UUID{"00000000-0000-0000-0000-000000000004"}},
        {DefaultMeshType::Sky, UUID{"00000000-0000-0000-0000-000000000005"}},
        {DefaultMeshType::FullscreenTriangle, UUID{"00000000-0000-0000-0000-000000000006"}},
    };

  private:
    std::shared_ptr<StaticMesh> CreateCubeMesh();
    std::shared_ptr<StaticMesh> CreateSphereMesh();
    std::shared_ptr<StaticMesh> CreatePlaneMesh();
    std::shared_ptr<StaticMesh> CreateCylinderMesh();
    std::shared_ptr<StaticMesh> CreateSkyMesh();
    std::shared_ptr<StaticMesh> CreateFullscreenTriangleMesh();

  public:
    MeshManager()
    {
        CreateDefault();
    }
    ~MeshManager() override
    {
        int a = 0;
    };
    void CreateDefault() override;
    std::shared_ptr<StaticMesh> GetMesh(DefaultMeshType type) const;
};

} // namespace MEngine::Resource