implementing ForwardOpaquePBR;

public static const float PI = 3.14159265359f;


public float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NOH = clamp(dot(N, H), 0.0, 1.0);
    float NOH2 = NOH * NOH;
    float nom = a2;
    float denom = (NOH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}

// --- 几何函数子项 ---
public float GeometrySchlickGGX(float NoV, float roughness)
{
    float r = roughness + 1.0;
    float k = r * r / 8.0;
    float nom = NoV;
    float denom = NoV * (1.0 - k) + k;
    return nom / denom;
}

// --- 几何函数 (G) ---
public float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    float NoL = clamp(dot(N, L), 0.0, 1.0);
    float ggx2 = GeometrySchlickGGX(NoV, roughness);
    float ggx1 = GeometrySchlickGGX(NoL, roughness);
    return ggx1 * ggx2;
}

// --- 菲涅尔方程 (F) ---
public float3 FresnelSchlick(float HoV, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - HoV, 0.0, 1.0), 5.0);
}

// --- 映射函数 ---
public float2 DirectionToUV(float3 dir)
{
    float3 d = normalize(dir);
    float u = atan2(d.z, d.x);
    u = (u + PI) / (2.0 * PI);
    float v = asin(d.y);
    v = (v + PI / 2.0) / PI;
    return float2(u, v);
}

// --- 低差异序列采样 (可选，如果你需要离线处理) ---
public float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

public float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i));
}
