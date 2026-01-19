#include "MeshManager.hpp"
#include "Logger.hpp"
#include <memory>

namespace MEngine::Resource
{
void MeshManager::CreateDefault()
{
    auto triangleMesh = CreateTriangle();
    auto cubeMesh = CreateCube();
    auto sphereMesh = CreateSphere();
    auto planeMesh = CreatePlane();
    auto cylinderMesh = CreateCylinder();
    auto skyMesh = CreateSky();
    auto fullscreenTriangleMesh = CreateFullscreenTriangle();
    triangleMesh->SetID(mDefaultMeshes[DefaultMeshType::Triangle]);
    cubeMesh->SetID(mDefaultMeshes[DefaultMeshType::Cube]);
    sphereMesh->SetID(mDefaultMeshes[DefaultMeshType::Sphere]);
    planeMesh->SetID(mDefaultMeshes[DefaultMeshType::Plane]);
    cylinderMesh->SetID(mDefaultMeshes[DefaultMeshType::Cylinder]);
    skyMesh->SetID(mDefaultMeshes[DefaultMeshType::Sky]);
    fullscreenTriangleMesh->SetID(mDefaultMeshes[DefaultMeshType::FullscreenTriangle]);
    Add(triangleMesh);
    Add(cubeMesh);
    Add(sphereMesh);
    Add(planeMesh);
    Add(cylinderMesh);
    Add(skyMesh);
    Add(fullscreenTriangleMesh);
}
std::shared_ptr<StaticMesh> MeshManager::CreateTriangle()
{
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 顶点1
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // 顶点2
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},   // 顶点3
    };
    const std::vector<uint32_t> indices = {
        0,
        2,
        1,
    };
    auto mesh = std::make_shared<StaticMesh>(DefaultMeshType::Triangle, vertices, indices);
    LogInfo("Created「triangle」mesh");
    return mesh;
}
std::shared_ptr<StaticMesh> MeshManager::CreateCube()
{
    // // Cube vertices and indices
    // // 左手系，Y轴向上，Z轴指向屏幕里内，X轴向右
    // // 逆时针为正面， 摄像机从-Z看向+Z方向
    // const std::vector<Vertex> vertices = {
    //     // 正面（-Z）
    //     {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},  // 0
    //     {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},   // 1
    //     {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 2
    //     {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},  // 3

    //     // 后面 (+Z)
    //     {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},   // 4
    //     {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 5
    //     {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 6
    //     {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // 7

    //     // 左面 (-X)
    //     {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},   // 8
    //     {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // 9
    //     {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // 10
    //     {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 11

    //     // 右面 (+X)
    //     {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 12
    //     {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},   // 13
    //     {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 14
    //     {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 15

    //     // 顶面 (+Y)
    //     {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 16
    //     {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // 17
    //     {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 18
    //     {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 19

    //     // 底面 (-Y)
    //     {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 20
    //     {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},  // 21
    //     {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},  // 22
    //     {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},   // 23
    // };

    // // 索引数据（每个面2个三角形，共36个索引，全部为逆时针顺序）
    // const std::vector<uint32_t> indices = {// 正面（-Z）
    //                                        0, 2, 1, 2, 3, 1,
    //                                        // 后面 (+Z)
    //                                        4, 6, 5, 6, 7, 5,
    //                                        // 左面(-X)
    //                                        8, 10, 9, 10, 11, 9,
    //                                        // 右面(+X)
    //                                        12, 14, 13, 14, 15, 13,
    //                                        // 顶面(+Y)
    //                                        16, 18, 17, 18, 19, 17,
    //                                        // 底面(-Y)
    //                                        20, 22, 21, 22, 23, 21};
    // Cube vertices and indices
    // 左手系，Y轴向上，Z轴指向屏幕里内，X轴向右
    // 逆时针为正面， 摄像机从-Z看向+Z方向
    const std::vector<Vertex> vertices = {
        // 正面（-Z）
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},  // 0
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},   // 1
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 2
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},  // 3

        // 后面 (+Z)
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},   // 4
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 5
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 6
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // 7

        // 左面 (-X)
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},   // 8
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // 9
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // 10
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 11

        // 右面 (+X)
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 12
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},   // 13
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 14
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 15

        // 顶面 (+Y)
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 16
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // 17
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 18
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 19

        // 底面 (-Y)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 20
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},  // 21
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},  // 22
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},   // 23
    };

    // 索引数据（每个面2个三角形，共36个索引，全部为逆时针顺序）
    const std::vector<uint32_t> indices = {// 正面（-Z）
                                           0, 2, 1, 2, 3, 1,
                                           // 后面 (+Z)
                                           4, 6, 5, 6, 7, 5,
                                           // 左面(-X)
                                           8, 10, 9, 10, 11, 9,
                                           // 右面(+X)
                                           12, 14, 13, 14, 15, 13,
                                           // 顶面(+Y)
                                           16, 18, 17, 18, 19, 17,
                                           // 底面(-Y)
                                           20, 22, 21, 22, 23, 21};
    auto mesh = std::make_shared<StaticMesh>(DefaultMeshType::Cube, vertices, indices);
    LogInfo("Created「cube」mesh");
    return mesh;
}
std::shared_ptr<StaticMesh> MeshManager::CreateSphere()
{
    const unsigned int sectorCount = 36; // 纬度
    const unsigned int stackCount = 18;  // 经度

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float radius = 0.5f; // 球体半径

    // 顶点计算
    for (unsigned int i = 0; i <= stackCount; ++i)
    {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount; // 计算每个纬度
        float xy = radius * cosf(stackAngle);                                        // 当前纬度半径
        float z = radius * sinf(stackAngle);                                         // Z轴坐标

        // 计算每个经度的顶点
        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = 2 * glm::pi<float>() * j / sectorCount; // 每个经度的角度

            // 计算顶点的坐标
            float x = xy * cosf(sectorAngle); // X坐标
            float y = xy * sinf(sectorAngle); // Y坐标
            vertices.push_back({
                {x, y, z},                                      // 位置
                {x / radius, y / radius, z / radius},           // 法线
                {float(j) / sectorCount, float(i) / stackCount} // UV坐标
            });
        }
    }

    // 索引计算
    for (unsigned int i = 0; i < stackCount; ++i)
    {
        unsigned int k1 = i * (sectorCount + 1); // 当前纬度的起始顶点
        unsigned int k2 = k1 + sectorCount + 1;  // 下一纬度的起始顶点

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    auto mesh = std::make_shared<StaticMesh>(DefaultMeshType::Sphere, vertices, indices);
    LogInfo("Created「sphere」mesh");
    return mesh;
}
std::shared_ptr<StaticMesh> MeshManager::CreatePlane()
{
    // // 平面顶点
    // const std::vector<Vertex> vertices = {
    //     // 顶面（Z+）
    //     {{-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // 0
    //     {{0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},   // 1
    //     {{0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // 2
    //     {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // 3
    // };

    const std::vector<Vertex> vertices = {
        // 正面(-Z)
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},  // 0
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},   // 1
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 2
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},  // 3
    };
    // 索引数据
    const std::vector<uint32_t> indices = {0, 2, 1, 2, 3, 1};

    auto mesh = std::make_shared<StaticMesh>(DefaultMeshType::Plane, vertices, indices);
    LogInfo("Created「plane」mesh");
    return mesh;
}
std::shared_ptr<StaticMesh> MeshManager::CreateCylinder()
{
    const unsigned int sectorCount = 36; // 圆周分段数
    const float radius = 0.5f;           // 圆柱半径
    const float halfHeight = 0.5f;       // 半高（圆柱总高度为1.0）

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (unsigned int i = 0; i <= 1; ++i)
    { // 上下两个圆环
        float y = i ? -halfHeight : halfHeight;
        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = 2 * glm::pi<float>() * j / sectorCount;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);

            // 法线在xz平面向外（Y轴为0）
            glm::vec3 normal(cosf(sectorAngle), 0.0f, sinf(sectorAngle));

            // UV坐标：u沿圆周，v沿高度
            float u = static_cast<float>(j) / sectorCount;
            float v = i ? 0.0f : 1.0f; // 顶部v=1.0，底部v=0.0

            vertices.push_back({
                {x, y, z}, // 位置
                normal,    // 法线
                {u, v}     // UV坐标
            });
        }
    }

    // 侧面索引（四边形带）
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        unsigned int topIndex = i;
        unsigned int bottomIndex = (sectorCount + 1) + i;

        // 第一个三角形
        indices.push_back(topIndex);
        indices.push_back(bottomIndex);
        indices.push_back(topIndex + 1);

        // 第二个三角形
        indices.push_back(topIndex + 1);
        indices.push_back(bottomIndex);
        indices.push_back(bottomIndex + 1);
    }
    unsigned int topCenterIndex = vertices.size();
    vertices.push_back({
        {0.0f, halfHeight, 0.0f}, // 中心点
        {0.0f, 1.0f, 0.0f},       // 法线向上
        {0.5f, 0.5f}              // UV中心
    });

    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float sectorAngle = 2 * glm::pi<float>() * i / sectorCount;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);

        // UV映射到圆盘 [0,1]
        float u = 0.5f + 0.5f * cosf(sectorAngle);
        float v = 0.5f + 0.5f * sinf(sectorAngle);

        vertices.push_back({
            {x, halfHeight, z}, // 在顶部平面
            {0.0f, 1.0f, 0.0f}, // 法线向上
            {u, v}              // UV坐标
        });
    }

    // 顶部索引
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        indices.push_back(topCenterIndex);
        indices.push_back(topCenterIndex + 1 + i);
        indices.push_back(topCenterIndex + 1 + i + 1);
    }
    unsigned int bottomCenterIndex = vertices.size();
    vertices.push_back({
        {0.0f, -halfHeight, 0.0f}, // 中心点
        {0.0f, -1.0f, 0.0f},       // 法线向下
        {0.5f, 0.5f}               // UV中心
    });

    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float sectorAngle = 2 * glm::pi<float>() * i / sectorCount;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);

        // UV映射到圆盘 [0,1] (与顶部相同)
        float u = 0.5f + 0.5f * cosf(sectorAngle);
        float v = 0.5f + 0.5f * sinf(sectorAngle);

        vertices.push_back({
            {x, -halfHeight, z}, // 在底部平面
            {0.0f, -1.0f, 0.0f}, // 法线向下
            {u, v}               // UV坐标
        });
    }

    // 底部索引（注意缠绕顺序翻转）
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomCenterIndex + 1 + i + 1);
        indices.push_back(bottomCenterIndex + 1 + i);
    }
    auto mesh = std::make_shared<StaticMesh>(DefaultMeshType::Cylinder, vertices, indices);
    LogInfo("Created「cylinder」mesh");
    return mesh;
}
std::shared_ptr<StaticMesh> MeshManager::CreateSky()
{
    const std::vector<Vertex> vertices = {
        // 前面
        {{-1.0f, -1.0f, 1.0f}, {}, {}},
        {{1.0f, -1.0f, 1.0f}, {}, {}},
        {{1.0f, 1.0f, 1.0f}, {}, {}},
        {{-1.0f, 1.0f, 1.0f}, {}, {}},
        // 后面
        {{-1.0f, -1.0f, -1.0f}, {}, {}},
        {{1.0f, -1.0f, -1.0f}, {}, {}},
        {{1.0f, 1.0f, -1.0f}, {}, {}},
        {{-1.0f, 1.0f, -1.0f}, {}, {}},
    };

    const std::vector<uint32_t> indices = {// 前面
                                           2, 1, 0, 0, 3, 2,
                                           // 右面
                                           6, 5, 1, 1, 2, 6,
                                           // 后面
                                           7, 4, 5, 5, 6, 7,
                                           // 左面
                                           3, 0, 4, 4, 7, 3,
                                           // 顶面
                                           6, 2, 3, 3, 7, 6,
                                           // 底面
                                           1, 5, 4, 4, 0, 1};

    auto mesh = std::make_shared<StaticMesh>(DefaultMeshType::Sky, vertices, indices);
    LogInfo("Created「sky」mesh");
    return mesh;
}
std::shared_ptr<StaticMesh> MeshManager::CreateFullscreenTriangle()
{
    const std::vector<Vertex> vertices = {
        // 全屏三角形顶点
        {{-1.0f, -1.0f, 0.0f}, {}, {0.0f, 0.0f}}, // 左下角
        {{3.0f, -1.0f, 0.0f}, {}, {2.0f, 0.0f}},  // 右下角
        {{-1.0f, 3.0f, 0.0f}, {}, {0.0f, 2.0f}}   // 左上角
    };

    const std::vector<uint32_t> indices = {0, 1, 2}; // 三角形索引

    auto mesh = std::make_shared<StaticMesh>(DefaultMeshType::FullscreenTriangle, vertices, indices);
    LogInfo("Created「fullscreen triangle」mesh");
    return mesh;
}
}; // namespace MEngine::Resource
