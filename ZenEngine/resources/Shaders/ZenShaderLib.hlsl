// define the ShaderGlobals uniform buffer
// this is always bound to binding 1 so $Global is automatically bound to 0
cbuffer ZenEngineGlobals : register(b1)
{
    float4x4 ZE_ViewProjectionMatrix;
    float4x4 ZE_InverseViewMatrix;
    float4x4 ZE_InverseProjectionMatrix;
    float4x4 ZE_ModelMatrix;
    float ZE_FarPlane;
    float ZE_NearPlane;
    
    float3 ZE_EyePosition;

    float3 ZE_AmbientLightColor;
    float ZE_AmbientLightIntensity;

    float3 ZE_DirectionalLightColor;
    float ZE_DirectionalLightIntensity;
    float3 ZE_DirectionalLightDirection;
};

#define ZE_MAX_SHININESS 256

float3 WorldPositionFromDepth(float depth, float2 texCoord)
{
    float z = depth * 2.0 - 1.0;

    float4 clipSpacePosition = float4(texCoord * 2.0 - 1.0, z, 1.0);
    float4 viewSpacePosition = mul(ZE_InverseProjectionMatrix, clipSpacePosition);
    viewSpacePosition /= viewSpacePosition.w;
    float4 worldSpacePosition = mul(ZE_InverseViewMatrix, viewSpacePosition);

    return worldSpacePosition.xyz;
}

float LinearizeDepth(float d)
{
    float zn = 2.0 * d - 1.0;
    return 2.0 * ZE_NearPlane * ZE_FarPlane / (ZE_FarPlane + ZE_NearPlane - zn * (ZE_FarPlane - ZE_NearPlane));
}

float3 PackNormals(float3 wsNormal)
{
    return (wsNormal + float3(1.0f, 1.0f, 1.0f)) / 2;
}

float3 UnpackNormals(float3 packedWsNormal)
{
    return (packedWsNormal * 2) - float3(1.0f, 1.0f, 1.0f);
}

float NormalizeShininess(float shininess)
{
    return shininess / ZE_MAX_SHININESS;
}

float GetShininessFromNormalizedValue(float nShininess)
{
    return nShininess * ZE_MAX_SHININESS;
}
