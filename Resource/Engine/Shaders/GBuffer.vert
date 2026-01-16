#version 460 core

layout(location = 0) in vec3 inPosition;  // Location 0
layout(location = 1) in vec3 inNormal;    // Location 1
layout(location = 2) in vec2 inTexCoords; // Location 2

layout(location = 2) out vec3 fragViewNormal;   // Location 2
layout(location = 3) out vec2 fragTexCoords;    // Location 3
layout(location = 4) out vec3 fragViewPosition; // Location 4
void main()
{
    fragTexCoords = inTexCoords;
    gl_Position = vec4(inPosition, 1.0);
}
