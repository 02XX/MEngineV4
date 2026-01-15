#pragma once
#include "Math.hpp"
#include <vulkan/vulkan.hpp>
namespace MEngine::Resource
{
class Vertex
{
  public:
    Vector3 position;
    Vector3 normal;
    Vector2 texCoords;
    static std::vector<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescription();
    static vk::VertexInputBindingDescription GetVertexInputBindingDescription();
    Vertex(Vector3 position, Vector3 normal, Vector2 texCoords)
        : position(position), normal(normal), texCoords(texCoords)
    {
    }
};
} // namespace MEngine::Resource