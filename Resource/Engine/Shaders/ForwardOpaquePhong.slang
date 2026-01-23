module ForwardOpaquePhong;
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
struct PhongParams
{
    float4 diffuse;
    float4 specular;
    float shininess;

    uint diffuseIndex;
    uint specularIndex;
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
ConstantBuffer<SceneParam> sceneParam;
[vk_binding(1, 0)]
StructuredBuffer<LightParam> lights;
[vk_binding(0, 1)]
Sampler2D textures[];
[vk_binding(0, 2)]
ConstantBuffer<PhongParams> phongParam;

struct VSInput
{
    [[vk_location(0)]]
    float3 position : POSITION;
    [[vk_location(1)]]
    float3 normal : NORMAL;
    [[vk_location(2)]]
    float2 uv : TEXCOORD;
};
struct FSInput
{
    float4 pos : SV_Position;
    [[vk_location(2)]]
    float3 fragNormal : NORMAL;
    [[vk_location(3)]]
    float2 fragUV : TEXCOORD;
    [[vk_location(4)]]
    float3 fragPosition : POSITION;
};
struct FSOutput
{
    [[vk_location(0)]]
    float4 color : SV_Target;
};

[shader("vertex")]
FSInput vertex(VSInput input)
{
    FSInput output;
    float4x4 modelMatrix = pc.modelMatrix;
    float4x4 viewMatrix = sceneParam.VMatrix;
    float4x4 projectionMatrix = sceneParam.projectionMatrix;
    float4 worldPosition = mul(float4(input.position, 1.0), modelMatrix);
    float4 viewPosition = mul(worldPosition, viewMatrix);
    float4 clipPosition = mul(viewPosition, projectionMatrix);
    output.pos = clipPosition;
    output.fragPosition = worldPosition.xyz;
    output.fragNormal = normalize(mul(float4(input.normal * 0.5 + 0.5, 0.0), modelMatrix).xyz);
    output.fragUV = input.uv;
    return output;
}

[shader("fragment")]
FSOutput fragment(FSInput input)
{
    FSOutput output;
    float3 N = input.fragNormal; // world space normal
    float3 diffuseColor = textures[NonUniformResourceIndex(phongParam.diffuseIndex)].Sample(input.fragUV).xyz * phongParam.diffuse.xyz;
    float3 specularColor = textures[NonUniformResourceIndex(phongParam.specularIndex)].Sample(input.fragUV).xyz * phongParam.specular.xyz;
    float3 I = float3(0.0, 0.0, 0.0);
    float3 Ia = float3(0.0, 0.0, 0.0); // Ambient light
    float3 Id = float3(0.0, 0.0, 0.0); // Diffuse light
    float3 Is = float3(0.0, 0.0, 0.0); // Specular light
    for (uint i = 0; i < sceneParam.lightCount; ++i)
    {
        LightParam light = lights[0];
        float3 L;
        float3 radiance;
        if (light.lightType == 0) // Directional Light
        {
            L = normalize(-light.direction.xyz);
            radiance = light.color.xyz * light.intensity;
        }
        else if (light.lightType == 1) // Point Light
        {
            L = normalize(light.position.xyz - input.fragPosition);
            float distance = length(light.position.xyz - input.fragPosition);
            float attenuation = saturate(1.0 - distance / light.radius);
            radiance = light.color.xyz * light.intensity * attenuation;
        }
        else if (light.lightType == 2) // Spot Light
        {
            L = normalize(light.position.xyz - input.fragPosition);
            float3 spotDir = normalize(-light.direction.xyz);
            float theta = dot(L, spotDir);
            float epsilon = light.innerCone - light.outerCone;
            float intensity = clamp((theta - light.outerCone) / epsilon, 0.0, 1.0);
            intensity *= light.intensity;
            light.intensity = intensity;
            float distance = length(light.position.xyz - input.fragPosition);
            float attenuation = saturate(1.0 - distance / light.radius);
            radiance = light.color.xyz * intensity * attenuation;
        }
        // I_ambient
        Ia += diffuseColor * radiance * 0.1; // assuming ambient factor is 0.1
        //  I_diffuse
        float NoL = dot(N, L);
        Id += diffuseColor * max(NoL, 0.0) * radiance;
        // I_specular
        float3 R = reflect(-L, N);
        float3 V = normalize(sceneParam.cameraPosition.xyz - input.fragPosition);
        Is += specularColor * pow(max(dot(R, V), 0.0), phongParam.shininess) * radiance;
    }
    I = Ia + Id + Is;
    output.color = float4(I, 1.0);
    return output;
}
