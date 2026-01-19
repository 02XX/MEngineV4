#include "Math.hpp"
#include <gtest/gtest.h>

TEST(AlignTest, alignStruct)
{
    struct LightParam
    {
        glm::vec4 Color;
        glm::vec4 Position;
        glm::vec4 Direction;

        uint32_t LightType;
        uint32_t Enable;
        float Intensity;
        float Radius;

        float InnerConeAngle;
        float OuterConeAngle;
    };

    LightParam a;
    std::printf("Size of LightParam: %zu\n", sizeof(LightParam));
    std::printf("Alignment of LightParam: %zu\n", alignof(LightParam));
}