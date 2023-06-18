// define the ShaderGlobals uniform buffer
// this is always bound to binding 1 so $Global is automatically bound to 0
cbuffer ZenEngineGlobals : register(b1)
{
    float4x4 ZE_ViewProjectionMatrix;
    float4x4 ZE_ModelMatrix;
};