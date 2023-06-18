cbuffer ZenEngineGlobals : register(b1)
{
    float4x4 ZE_ViewProjectionMatrix;
    float4x4 ZE_ModelMatrix;
};

struct VertexInfo
{
    float3 Position : POSITION;
};

struct Vertex2Pixel
{
    float4 Position : SV_POSITION;
};

float4 Color;

// vertex shader function
Vertex2Pixel VSMain(VertexInfo input)
{
    Vertex2Pixel output;
    output.Position = mul(ZE_ModelMatrix, mul(ZE_ViewProjectionMatrix, float4(input.Position, 1.0f)));
    return output;
}

// pixel shader function
float4 PSMain(Vertex2Pixel input) : SV_Target0
{
    return Color;
}