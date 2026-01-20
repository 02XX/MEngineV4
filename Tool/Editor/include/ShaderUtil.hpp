#pragma once
#include "Math.hpp"
#include <imgui.h>
#include <vector>
namespace MEngine::Tool::Utils
{
inline Matrix4 viewMatrix{};
inline Matrix4 projectionMatrix{};
inline ImVec2 viewportPos{0.0f, 0.0f};
inline ImVec2 viewportSize{800.0f, 600.0f};
inline void SetViewPort(const ImVec2 &pos, const ImVec2 &size)
{
    viewportPos = pos;
    viewportSize = size;
}
inline void SetMatrix(const Matrix4 &view, const Matrix4 &projection)
{
    viewMatrix = view;
    projectionMatrix = projection;
}
inline ImVec2 WorldToScreen(const Vector3 &worldPos, const Matrix4 &viewMatrix, const Matrix4 &projectionMatrix,
                            const ImVec2 &viewportPos, const ImVec2 &viewportSize)
{
    Vector4 clipSpacePos = projectionMatrix * viewMatrix * Vector4(worldPos, 1.0f);
    Vector3 ndcSpacePos = Vector3(clipSpacePos) / (clipSpacePos.w + glm::epsilon<float>());
    ImVec2 screenPos;
    screenPos.x = ((ndcSpacePos.x + 1.0f) / 2.0f) * viewportSize.x;
    screenPos.y = ((1.0f - ndcSpacePos.y) / 2.0f) * viewportSize.y; // Invert Y for ImGui
    screenPos.x += viewportPos.x;
    screenPos.y += viewportPos.y;
    return screenPos;
}
inline ImVec2 ClampToViewport(const ImVec2 &screenPos, const ImVec2 &viewportPos, const ImVec2 &viewportSize)
{
    ImVec2 clampedPos;
    clampedPos.x = glm::clamp(screenPos.x, viewportPos.x, viewportPos.x + viewportSize.x);
    clampedPos.y = glm::clamp(screenPos.y, viewportPos.y, viewportPos.y + viewportSize.y);
    return clampedPos;
}
inline void DrawLine(Vector3 from, Vector3 to, ImU32 color, float thickness = 1.0f)
{
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 p1 = WorldToScreen(from, viewMatrix, projectionMatrix, viewportPos, viewportSize);
    ImVec2 p2 = WorldToScreen(to, viewMatrix, projectionMatrix, viewportPos, viewportSize);

    drawList->AddLine(p1, p2, color, thickness);
}
inline void DrawCircle(Vector3 center, Vector3 normal, float radius, ImU32 color, int segments = 64,
                       float thickness = 1.0f)
{
    ImDrawList *drawList = ImGui::GetWindowDrawList();

    // 1. 严格归一化法线，防止缩放误差
    Vector3 n = glm::normalize(normal);

    // 2. 构造严格正交基 (使用更稳定的投影方式)
    // 即使 normal 很接近某个轴，这段逻辑也能保证生成的 tangent 和 bitangent 绝对垂直且长度为 1
    Vector3 w = (glm::abs(n.z) < 0.999f) ? Vector3(0, 0, 1) : Vector3(0, 1, 0);
    Vector3 tangent = glm::normalize(glm::cross(w, n));
    Vector3 bitangent = glm::cross(n, tangent); // 不需要再 normalize，因为 n 和 tangent 已正交且归一化

    // 3. 核心：统一起始旋转量 (Phase Alignment)
    // 如果没有这一步，画垂直的圆时，圆 A 的顶点可能正对着圆 B 的线段中间，导致视觉上的“超出”
    for (int i = 0; i < segments; ++i)
    {
        // 建议使用 double 提升中间计算精度，最后再转回 float
        float t1 = (float)i / (float)segments * 2.0f * 3.1415926535f;
        float t2 = (float)(i + 1) / (float)segments * 2.0f * 3.1415926535f;

        // 计算本地坐标
        Vector3 localP1(glm::cos(t1) * radius, glm::sin(t1) * radius, 0.0f);
        Vector3 localP2(glm::cos(t2) * radius, glm::sin(t2) * radius, 0.0f);

        // 变换到世界坐标
        Vector3 p1 = center + (tangent * localP1.x + bitangent * localP1.y);
        Vector3 p2 = center + (tangent * localP2.x + bitangent * localP2.y);

        DrawLine(p1, p2, color, thickness);
    }
}
inline void DrawSphere(Vector3 center, float radius, ImU32 color,
                       int segments = 64, // 每个圆环的平滑度
                       int latLines = 8,  // 纬线数量 (横向环)
                       int lonLines = 8,  // 经线数量 (纵向圆)
                       float thickness = 1.0f)
{
    // 1. 绘制纬线 (Horizontal Rings)
    // 纬线是水平的，法线始终为 (0, 1, 0)
    for (int i = 1; i <= latLines; ++i)
    {
        // 计算从球顶到球底的角度 phi (0 到 PI)
        float phi = glm::pi<float>() * (static_cast<float>(i) / static_cast<float>(latLines + 1));

        // 当前高度分量 (Y 轴偏移)
        float verticalOffset = radius * glm::cos(phi);
        // 当前切面的圆半径
        float ringRadius = radius * glm::sin(phi);

        Vector3 ringCenter = center + Vector3(0, verticalOffset, 0);
        Vector3 ringNormal = Vector3(0, 1, 0); // 垂直向上

        DrawCircle(ringCenter, ringNormal, ringRadius, color, segments, thickness);
    }

    // 2. 绘制经线 (Vertical Rings)
    // 经线是垂直穿过南北极的，圆心始终是球心，半径始终是 radius
    // 变化的是圆平面的法线（在 XZ 平面上旋转）
    for (int i = 0; i < lonLines; ++i)
    {
        // 只需要旋转 180 度即可，因为 DrawCircle 画的是 360 度的全圆
        float theta = glm::pi<float>() * (static_cast<float>(i) / static_cast<float>(lonLines));

        // 计算法线向量：在 XZ 平面上旋转
        // 当 theta = 0, 法线是 (1, 0, 0)，圆在 YZ 平面
        // 当 theta = PI/2, 法线是 (0, 0, 1)，圆在 XY 平面
        Vector3 ringNormal = Vector3(glm::cos(theta), 0, glm::sin(theta));

        DrawCircle(center, ringNormal, radius, color, segments, thickness);
    }
}
} // namespace MEngine::Tool::Utils