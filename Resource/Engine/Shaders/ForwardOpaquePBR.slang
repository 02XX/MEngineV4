
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
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4 cameraPosition;
    
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
struct VSOutput
{
    float4 pos : SV_Position;
    [[vk_location(2)]]
    float3 viewNormal : NORMAL;
    [[vk_location(3)]]
    float2 texCoord : TEXCOORD;
    [[vk_location(4)]]
    float3 viewPosition : POSITION;
};
struct FSOutput
{
    [[vk_location(0)]]
    float4 color : SV_Target;
};

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    float4x4 modelMatrix = pc.modelMatrix;
    float4x4 viewMatrix = sceneParam.viewMatrix;
    float4x4 projectionMatrix = sceneParam.projectionMatrix;
    output.texCoord = input.texCoord;
    float4 worldPosition = mul(float4(input.position, 1.0), modelMatrix);
    float4 viewPosition = mul(worldPosition, viewMatrix);
    float4 clipPosition = mul(viewPosition, projectionMatrix);
    output.viewPosition = viewPosition.xyz;
    output.pos = clipPosition;
    return output;
}

[shader("fragment")]
FSOutput fragmentMain(VSOutput input)
{
    FSOutput output;
    uint albedoIdx = pbrProperties.albedoIndex;
    float4 sampledColor = textures[NonUniformResourceIndex(albedoIdx)].Sample(input.texCoord);
    float4 color = sampledColor * pbrProperties.albedo;
    output.color = color;
    return output;
}
