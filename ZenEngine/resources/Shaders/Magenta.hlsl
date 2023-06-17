struct VertexInfo
{
    [[vk::location(0)]]
    float3 Position : POSITION;
};

struct Vertex2Pixel
{
    float4 Position : SV_POSITION;
};

// vertex shader function
Vertex2Pixel VSMain(VertexInfo input)
{
    Vertex2Pixel output;
    output.Position = float4(input.Position, 1.0f);
    return output;
}

// pixel shader function
float4 PSMain(Vertex2Pixel input) : SV_Target0
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}