#include "Common.hlsli"

struct PixelOutput
{
    float4 pos : SV_POSITION;
    float4 worldPosition : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 uv : TEXCOORD;
    float clip : SV_ClipDistance0;
};

PixelOutput main(VertexInput input)
{
    PixelOutput output;

    float4 vertexObjectPos = float4(input.pos, 1.0f);
    
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    float4 vertexObjectNormal = float4(input.normal, 0.0);
    float3 vertexWorldNormal = mul((float3x3) modelToWorld, input.normal);

    
    output.pos = vertexClipPos;
    output.worldPosition = vertexWorldPos;
    output.normal = normalize(vertexWorldNormal);
    output.tangent = normalize(mul((float3x3) modelToWorld, input.tangent));
    output.bitangent = normalize(mul((float3x3) modelToWorld, input.bitangent));
    output.uv = input.uv;
    
    output.clip = input.pos.y - clipHeight;
    
    return output;
}

