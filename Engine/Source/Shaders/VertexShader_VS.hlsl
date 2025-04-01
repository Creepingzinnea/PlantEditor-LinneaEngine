#include "Common.hlsli"

struct PixelOutputType
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

PixelOutputType main(VertexInput input)
{
    PixelOutputType output;
    float3 vertexObjectPos = input.pos;
    
    float4 vertexWorldPos = mul(modelToWorld, float4(vertexObjectPos, 1));
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.pos = vertexClipPos;
    output.uv = input.uv;
    output.normal = input.normal;
    return output;
}

