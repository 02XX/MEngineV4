#pragma once
#include "Manager.hpp"
#include "StaticMesh.hpp"
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
class MeshManager final : public Manager<StaticMesh>, public virtual IManager<StaticMesh>
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
    std::shared_ptr<StaticMesh> CreateTriangle();
    std::shared_ptr<StaticMesh> CreateCube();
    std::shared_ptr<StaticMesh> CreateSphere();
    std::shared_ptr<StaticMesh> CreatePlane();
    std::shared_ptr<StaticMesh> CreateCylinder();
    std::shared_ptr<StaticMesh> CreateSky();
    std::shared_ptr<StaticMesh> CreateFullscreenTriangle();

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
};

} // namespace MEngine::Resource