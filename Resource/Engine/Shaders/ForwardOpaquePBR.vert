#version 460 core
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : enable

layout(location = 0) in vec3 inPosition;  // Location 0
layout(location = 1) in vec3 inNormal;    // Location 1
layout(location = 2) in vec2 inTexCoords; // Location 2

layout(location = 2) out vec3 fragViewNormal;   // Location 2
layout(location = 3) out vec2 fragTexCoords;    // Location 3
layout(location = 4) out vec3 fragViewPosition; // Location 4
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
layout(push_constant) uniform PC
{
    mat4 modelMatrix;
}
pc;
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
void main()
{
    mat4 modelMatrix = pc.modelMatrix;
    mat4 viewMatrix = sceneBuffer.viewMatrix;
    mat4 projectionMatrix = sceneBuffer.projectionMatrix;

    fragTexCoords = inTexCoords;

    vec4 worldPosition = modelMatrix * vec4(inPosition, 1.0);
    vec4 viewPosition = viewMatrix * worldPosition;
    fragViewPosition = viewPosition.xyz;
    fragViewNormal = mat3(transpose(inverse(viewMatrix * modelMatrix))) * inNormal;
    gl_Position = projectionMatrix * viewPosition;
}
