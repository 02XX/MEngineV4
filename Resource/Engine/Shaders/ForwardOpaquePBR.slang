module ForwardOpaquePBR;
__include PBRUtils;
struct LightParam
{
    float4 color;
    float4 position;
    float4 direction;
    uint lightType;
    uint enable;
    float intensity;
    float radius;
    float innerCone;
    float outerCone;
};
struct PBRProperties
{
    float4 albedo;
    float4 normal;
    float metallic;
    float roughness;
    float ao;
    float emissive;
    uint albedoIndex;
    uint normalIndex;
    uint armIndex;
    uint emissiveIndex;

    uint albedoBindlessIndex;
    uint normalBindlessIndex;
    uint ARMBindlessIndex;
    uint emissiveBindlessIndex;
};
struct SceneParam
{
    float4x4 VMatrix;
    float4x4 projectionMatrix;
    float4 cameraPosition;
    float4 cameraDirection;
    uint lightCount;
};
struct PushConstants
{
    float4x4 modelMatrix;
};
[vk_push_constant]
ConstantBuffer<PushConstants> pc;
// (binding, set)
[vk_binding(0, 0)]
Sampler2D textures[];
[vk_binding(0, 1)]
ConstantBuffer<SceneParam> sceneParam;
[vk_binding(1, 1)]
StructuredBuffer<LightParam> lights;
[vk_binding(0, 2)]
ConstantBuffer<PBRProperties> pbrProperties;

struct VSInput
{
    [[vk_location(0)]]
    float3 position : POSITION;
    [[vk_location(1)]]
    float3 normal : NORMAL;
    [[vk_location(2)]]
    float2 texCoord : TEXCOORD;
};
struct FSInput
{
    float4 pos : SV_Position;
    [[vk_location(2)]]
    float3 VNormal : NORMAL;
    [[vk_location(3)]]
    float2 texCoord : TEXCOORD;
    [[vk_location(4)]]
    float3 VPosition : POSITION;
};
struct FSOutput
{
    [[vk_location(0)]]
    float4 color : SV_Target;
};

[shader("vertex")]
FSInput vertexMain(VSInput input)
{
    FSInput output;
    float4x4 modelMatrix = pc.modelMatrix;
    float4x4 VMatrix = sceneParam.VMatrix;
    float4x4 projectionMatrix = sceneParam.projectionMatrix;
    output.texCoord = input.texCoord;
    float4 worldPosition = mul(float4(input.position, 1.0), modelMatrix);
    float4 VPosition = mul(worldPosition, VMatrix);
    float4 clipPosition = mul(VPosition, projectionMatrix);
    output.VPosition = VPosition.xyz;
    output.pos = clipPosition;
    output.VNormal = normalize(mul(float4(input.normal, 0.0), modelMatrix).xyz);
    output.texCoord = input.texCoord;

    return output;
}

[shader("fragment")]
FSOutput fragmentMain(FSInput input)
{
    FSOutput output;
    uint albedoIdx = pbrProperties.albedoIndex;
    uint normalIdx = pbrProperties.normalIndex;
    uint armIdx = pbrProperties.armIndex;
    uint emissiveIdx = pbrProperties.emissiveIndex;

    float3 albedo = textures[NonUniformResourceIndex(albedoIdx)].Sample(input.texCoord).rgb * pbrProperties.albedo.rgb;
    float3 normalMap = textures[NonUniformResourceIndex(normalIdx)].Sample(input.texCoord).rgb * pbrProperties.normal.rgb;

    float ao = pbrProperties.ao;
    float roughness = pbrProperties.roughness;
    float metallic = pbrProperties.metallic;
    float3 emissive = pbrProperties.emissive;

    float3 V = -input.VPosition; // 相机空间
    float3 N = normalize(input.VNormal);
    float3 F0 = float3(0.04);
    F0 = lerp(F0, albedo, metallic);
    float3 Lo = float3(0.0);
    // for (uint i = 0; i < sceneParam.lightCount; ++i)
    // {
    LightParam light = lights[0];
    float3 L;
    float3 radiance;
    float attenuation = 1.0;
    if (light.lightType == 0) // 平行光
    {
        L = normalize(-mul(float4(light.direction.xyz, 0.0), sceneParam.VMatrix).xyz);
        radiance = light.color.rgb * light.intensity;
    }
    else if (light.lightType == 1) // 点光源
    {
        float3 lightPosV = mul(float4(light.position.xyz, 1.0), sceneParam.VMatrix).xyz;
        L = normalize(lightPosV - input.VPosition);
        float distance = length(lightPosV - input.VPosition);
        attenuation = saturate(1.0 - distance / light.radius);
        radiance = light.color.rgb * light.intensity * attenuation;
    }
    else if (light.lightType == 2) // 聚光灯
    {
        float3 lightPosV = mul(float4(light.position.xyz, 1.0), sceneParam.VMatrix).xyz;
        L = normalize(lightPosV - input.VPosition);
        float distance = length(lightPosV - input.VPosition);
        attenuation = saturate(1.0 - distance / light.radius);
        float3 spotDirV = normalize(mul(float4(light.direction.xyz, 0.0), sceneParam.VMatrix).xyz);
        float theta = dot(L, -spotDirV);
        float epsilon = cos(radians(light.innerCone)) - cos(radians(light.outerCone));
        float spotIntensity = saturate((theta - cos(radians(light.outerCone))) / epsilon);
        attenuation *= spotIntensity;
        radiance = light.color.rgb * light.intensity * attenuation;
    }
    float3 H = normalize(V + L);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    float3 specular = numerator / denominator;

    float3 kS = F;
    float3 kD = (float3(1.0) - kS) * (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    // }
    // // 4. 环境光与合并
    // float3 ambient = float3(0.03) * albedo * ao;
    // float3 color = ambient + Lo + emissive;

    // // 5. 后处理 (简单的 Reinhard Tone Mapping)
    // color = color / (color + float3(1.0));
    // color = pow(color, float3(1.0 / 2.2));

    output.color = float4(input.VNormal * 0.5 + 0.5, 1.0);
    return output;
}
