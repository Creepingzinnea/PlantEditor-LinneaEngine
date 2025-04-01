#include "Common.hlsli"
#include "PBRFunctions.hlsli"

Texture2D skyBox : register(t11);

struct PixelInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};


PixelOutput main(PixelInput input)
{
    PixelOutput output;
    float3 materialColor = skyBox.Sample(samplerState, input.uv);
    output.color = float4(materialColor, 1.0f);
    return output;
}