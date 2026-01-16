#version 460 core
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : enable
layout(location = 0) out vec4 OutColor;

layout(location = 2) in vec3 fragViewNormal;   // Location 2
layout(location = 3) in vec2 fragTexCoord;     // Location 3
layout(location = 4) in vec3 fragViewPosition; // Location 4

layout(set = 0, binding = 0) uniform sampler2D textures[];
layout(buffer_reference, std430) buffer pbrPropertiesBuffer
{
    vec4 albedo;
    vec4 normal;
    float metallic;
    float roughness;
    float ao;
    float emissive;
    uint albedoIndex;
    uint normalIndex;
    uint ARMIndex;
    uint emissiveIndex;
};

layout(push_constant) uniform PC
{
    mat4 modelMatrix;
    uint64_t sceneAddr;
    uint64_t materialAddr;
}
pc;

void main()
{
    uint64_t targetAddr = pc.materialAddr;
    pbrPropertiesBuffer mat = pbrPropertiesBuffer(targetAddr);
    uint albedoIndex = mat.albedoIndex;
    vec4 color = texture(textures[nonuniformEXT(albedoIndex)], fragTexCoord) * mat.albedo;
    vec4 texCoordColor = vec4(fragTexCoord.x, fragTexCoord.y, 0.0, 1.0);
    OutColor = texCoordColor;
}
