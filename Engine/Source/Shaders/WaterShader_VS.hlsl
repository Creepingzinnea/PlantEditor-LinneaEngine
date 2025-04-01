#include "Common.hlsli"

struct PixelOutput
{
    float4 pos : SV_POSITION;
    float4 worldPosition : POSITION;
    float2 uv : TEXCOORD;
};

PixelOutput main(VertexInput input)
{
    PixelOutput output;

    float4 vertexObjectPos = float4(input.pos, 1.0f);
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);

    output.worldPosition = vertexWorldPos;
    output.pos = vertexClipPos;
    output.uv = input.uv;

    return output;
}


