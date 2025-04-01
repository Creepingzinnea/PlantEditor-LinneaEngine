//Documentation---------------------------------------------------
//This is a common constant buffer that all shaders can acces from
//The data must be identical to the BufferData header which is the CPU-side for this buffer. 
//must be 16 byte aligned
//----------------------------------------------------------------

TextureCube environmentTexture : register(t9);

struct VertexInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 uv : TEXCOORD;
};

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelToWorld;
}

cbuffer sFrameBuffer : register(b0)
{
    float4x4 myWorldToDirectionalLightTransform;        //64 bytes
    float4x4 worldToClipMatrix;                         //64 bytes      
    float4 directionalLightColorAndIntensity;           //16 bytes
    float4 ambientLightColorAndIntensity;               //16 bytes
    
    float3 directionalLightDirection;                   //8 bytes
    float padding;                                      // +8 = 16 bytes
    
    float3 cameraPosition;                              //12 bytes   
    float padding1;                                     // +8 = 16 bytes
    
    uint2 resolution;                                   //8 bytes  
    float deltaTime;                                    //4 bytes
    float totalTime;                                    //4 bytes
             
    float clipHeight;                                   //4 bytes
    float3 padding2;                                    //12 bytes
};

// Origin: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}


float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}