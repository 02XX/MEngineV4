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
    Vector3 texCoords;
    static std::vector<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescription();
    static vk::VertexInputBindingDescription GetVertexInputBindingDescription();
};
} // namespace MEngine::Resource