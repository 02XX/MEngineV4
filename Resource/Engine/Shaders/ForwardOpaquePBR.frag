#version 460 core
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : enable
layout(location = 0) out vec4 OutColor;

layout(location = 2) in vec3 fragViewNormal;   // Location 2
layout(location = 3) in vec2 fragTexCoord;     // Location 3
layout(location = 4) in vec3 fragViewPosition; // Location 4

layout(push_constant) uniform PC
{
    mat4 modelMatrix;
}
pc;

struct LightParam
{
    vec4 Color;
    vec4 Position;
    vec4 Direction;
    uint LightType;
    uint Enable;
    float Intensity;
    float Radius;
    float InnerCone;
    float OuterCone;
};

layout(set = 0, binding = 0) uniform sampler2D textures[];
layout(set = 1, binding = 0, std430) buffer SceneBuffer
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec4 cameraPosition;
    uint NumLights;
    LightParam Lights[];
}
sceneBuffer;
layout(set = 2, binding = 0, std430) buffer PBRPropertiesBuffer
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

    uint albedoBindlessIndex;
    uint normalBindlessIndex;
    uint ARMBindlessIndex;
    uint emissiveBindlessIndex;
}
pbrPropertiesBuffer;

void main()
{
    uint albedoIndex = pbrPropertiesBuffer.albedoIndex;
    vec4 color = texture(textures[nonuniformEXT(albedoIndex)], fragTexCoord) * pbrPropertiesBuffer.albedo;
    vec4 texCoordColor = vec4(fragTexCoord.x, fragTexCoord.y, 0.0, 1.0);
    LightParam light = sceneBuffer.Lights[0];
    OutColor = color;
}
