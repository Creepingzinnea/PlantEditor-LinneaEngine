#include "Common.hlsli"
#include "PBRFunctions.hlsli"

Texture2D aTexture : register(t10);

struct PixelInputType
{
    float4 pos : SV_POSITION;
    float4 worldPosition : POSITION;
    float2 uv : TEXCOORD;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};


PixelOutput main(PixelInputType input)
{
    float Amplitude = 0.0002f;
    float Frequency = 0.5f;
    float Speed = 2.0f;
    //----------------------------------------------------------------------------------------------------
    
    PixelOutput result;
    float3 toEye = normalize(cameraPosition - input.worldPosition.xyz);
    float width = 0.0f;
    float height = 0.0f;
    aTexture.GetDimensions(width, height);
    float2 resolution = float2(width, height);
    

    
    //----------------------------------------------------------------------------------------------------
    
    float dist = max(length(input.worldPosition.xz - cameraPosition.xz), 1.0f); // Ensure minimum distance
    float2 p = input.worldPosition.xz;
    
    // Use more wave vectors with different frequencies and directions for more variety
    float2 k0 = float2(6.f, 16.f) * Frequency;
    float2 k1 = float2(10.f, -14.f) * Frequency;
    float2 k2 = float2(13.f, 7.f) * Frequency;
    float2 k3 = float2(-8.f, 11.f) * Frequency;
    
    // Add phase shifts and different speeds for more organic look
    float t0 = totalTime * 0.8f * Speed;
    float t1 = totalTime * 1.2f * Speed;
    float t2 = totalTime * 0.7f * Speed;
    float t3 = totalTime * 1.1f * Speed;
    
    // Sum multiple wave patterns with different phases
    float2 heightDerivative =
        k0 * sin(dot(p, k0) + t0) +
        k1 * sin(dot(p, k1) + t1) +
        k2 * sin(dot(p, k2) + t2) * 0.7f +
        k3 * sin(dot(p, k3) + t3) * 0.5f;
    
    // Calculate max value based on all wave vectors
    float2 maxValue = float2(0.f, length(k0) + length(k1) + 0.7f * length(k2) + 0.5f * length(k3));
    
    // Add noise based on position for more variation
    float noise = sin(p.x * 0.2f) * sin(p.y * 0.2f) * 0.5f + 0.5f;
    
    // Calculate offset with smooth falloff based on distance
    float falloff = 1.0f / (1.0f + 0.1f * dist);
    float2 offset = Amplitude * (maxValue + heightDerivative) * falloff * (1.0f + noise * 0.2f);
    
    // Calculate more accurate wave normal based on height derivatives
    float3 waveNormal = normalize(float3(-heightDerivative.x * Amplitude, 1.0, -heightDerivative.y * Amplitude));
    
    // Calculate fresnel with the wave normal for more realistic water surface
    float fresnel = Fresnel_Schlick
    (
        float3(0.25f, 0.25f, 0.25f),
        waveNormal,
        toEye
    );
    
    // Sample with offset and apply fresnel-----------------------------------------------------------------------------------------
    float3 reflection = aTexture.Sample(samplerState, input.pos.xy / resolution + offset).rgb;
    result.color.rgb = fresnel * reflection;
    result.color.a = reflection * waveNormal.y;
    
    
    //------------------------------------------------------------------------------------------------------------------------------
    return result;
}