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

layout(buffer_reference, std430) buffer sceneBuffer
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 cameraPosition;
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
    uint64_t sceneAddress = pc.sceneAddr;
    sceneBuffer sb = sceneBuffer(sceneAddress);

    mat4 modelMatrix = pc.modelMatrix;
    mat4 viewMatrix = sb.viewMatrix;
    mat4 projectionMatrix = sb.projectionMatrix;

    fragTexCoords = inTexCoords;

    vec4 worldPosition = modelMatrix * vec4(inPosition, 1.0);
    vec4 viewPosition = viewMatrix * worldPosition;
    fragViewPosition = viewPosition.xyz;
    fragViewNormal = mat3(transpose(inverse(viewMatrix * modelMatrix))) * inNormal;
    gl_Position = projectionMatrix * viewPosition;
}
