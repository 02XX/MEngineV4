#include "Math.hpp"
#include <gtest/gtest.h>
#include <print>
TEST(MVPTest, WorldToScreenTest)
{
    // 左手系 Up为+Y
    Vector4 lineStartWorld = Vector4(0.0f, 0.0f, 0.0f, 1.0f), lineEndWorld = Vector4(-10.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Matrix4 viewMatrix = glm::lookAtLH(cameraPos, cameraTarget, cameraUp);
    Vector4 lineStartView = viewMatrix * lineStartWorld;
    Vector4 lineEndView = viewMatrix * lineEndWorld;

    float fovY = glm::radians(120.0f);
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 3.0f;
    float farPlane = 10.0f;
    Matrix4 projectionMatrix = glm::perspectiveLH_ZO(fovY, aspectRatio, nearPlane, farPlane);
    Vector4 lineStartClip = projectionMatrix * lineStartView;
    Vector4 lineEndClip = projectionMatrix * lineEndView;
    Vector4 lineStartNDC = lineStartClip / (lineStartClip.w + glm::epsilon<float>());
    Vector4 lineEndNDC = lineEndClip / (lineEndClip.w + glm::epsilon<float>());

    Vector2 viewportSize = Vector2(160.0f, 90.0f);
    Vector2 lineStartScreen;
    lineStartScreen.x = ((lineStartNDC.x + 1.0f) / 2.0f) * viewportSize.x;
    lineStartScreen.y = ((1.0f - lineStartNDC.y) / 2.0f) * viewportSize.y; // Invert Y for screen space
    Vector2 lineEndScreen;
    lineEndScreen.x = ((lineEndNDC.x + 1.0f) / 2.0f) * viewportSize.x;
    lineEndScreen.y = ((1.0f - lineEndNDC.y) / 2.0f) * viewportSize.y; // Invert Y for screen space
    if (lineEndNDC.w < 0.01f)
    {
        std::println("Line end is behind the camera and not visible on screen.");
    }

    std::println("World Space: ({},{},{},{}) to ({},{},{},{})", lineStartWorld.x, lineStartWorld.y, lineStartWorld.z,
                 lineStartWorld.w, lineEndWorld.x, lineEndWorld.y, lineEndWorld.z, lineEndWorld.w);
    std::println("View Space: ({},{},{},{}) to ({},{},{},{})", lineStartView.x, lineStartView.y, lineStartView.z,
                 lineStartView.w, lineEndView.x, lineEndView.y, lineEndView.z, lineEndView.w);
    std::println("Clip Space: ({},{},{},{}) to ({},{},{},{})", lineStartClip.x, lineStartClip.y, lineStartClip.z,
                 lineStartClip.w, lineEndClip.x, lineEndClip.y, lineEndClip.z, lineEndClip.w);
    std::println("NDC Space: ({},{},{},{}) to ({},{},{},{})", lineStartNDC.x, lineStartNDC.y, lineStartNDC.z,
                 lineStartNDC.w, lineEndNDC.x, lineEndNDC.y, lineEndNDC.z, lineEndNDC.w);
    std::println("Screen Space: ({},{}) to ({},{})", lineStartScreen.x, lineStartScreen.y, lineEndScreen.x,
                 lineEndScreen.y);
}