#include "ZenShaderLib.hlsl"

struct Vertex
{
    float2 Position: POSITION;
};

struct Interpolators
{
    float4 Position: SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

Texture2D BaseColorSpecular: register(t0);
SamplerState BaseColorSpecular_Sampler: register(s0);

Texture2D Normal: register(t1);
SamplerState Normal_Sampler: register(s1);

Texture2D Shininess : register(t2);
SamplerState Shininess_Sampler : register(t2);

Texture2D Depth: register(t3);
SamplerState Depth_Sampler: register(s3);

Interpolators VSMain(Vertex v)
{
    Interpolators i;
    i.Position = float4(v.Position, 0.0f, 1.0f);
    i.TexCoord = 0.5f * (v.Position + float2(1.0f, 1.0f));
    return i;
}

float4 PSMain(Interpolators i)
{
    float4 baseColorSpecular = BaseColorSpecular.Sample(BaseColorSpecular_Sampler, i.TexCoord);
    float4 shininessSample = Shininess.Sample(Shininess_Sampler, i.TexCoord);

    float depth = Depth.Sample(Depth_Sampler, i.TexCoord).r;
    float3 wsPosition = WorldPositionFromDepth(depth, i.TexCoord);
    float3 normal = UnpackNormals(Normal.Sample(Normal_Sampler, i.TexCoord).xyz);
    float3 baseColor = baseColorSpecular.rgb;
    float specular = baseColorSpecular.a;
    float shininess = GetShininessFromNormalizedValue(shininessSample.r);

    // Ambient light
    float3 ambientLight = ZE_AmbientLightColor * ZE_AmbientLightIntensity;

    // Diffuse light
    float directionalFactor = max(dot(normal, -ZE_DirectionalLightDirection), 0.0);
    float3 diffuseLight = (directionalFactor * ZE_DirectionalLightIntensity) * ZE_DirectionalLightColor;

    // Specular light
    float3 viewDirection = normalize(ZE_EyePosition - wsPosition);
    float3 directionalLightReflectDirection = reflect(ZE_DirectionalLightDirection, normal);
    float specularFactor = max(dot(viewDirection, directionalLightReflectDirection), 0.0);
    specularFactor = specular * pow(specularFactor, shininess);
    float3 specularLight = (specularFactor * ZE_DirectionalLightIntensity) * ZE_DirectionalLightColor;
    
    float3 color = (ambientLight + diffuseLight + specularLight) * baseColor;
    return float4(color, 1.0f);
}