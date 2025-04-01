#include "Common.hlsli"
#include "PBRFunctions.hlsli"

Texture2D grassTexture : register(t0);
Texture2D grassMaterial : register(t6);
Texture2D grassNormalMap : register(t3);

Texture2D rockTexture : register(t1);
Texture2D rockMaterial : register(t7);
Texture2D rockNormalMap : register(t4);

Texture2D sandTexture : register(t2);
Texture2D sandMaterial : register(t8);
Texture2D sandNormalMap : register(t5);

cbuffer sTerrainBuffer : register(b2)
{
    float heightBlendMin; // 4 bytes
    float heightBlendMax; // 4 bytes
    float slopeBlendMin; // 4 bytes
    float slopeBlendMax; // 4 bytes

    float blendStrength; // 4 bytes
    float3 padding3; // 12 bytes padding to align to 16-byte boundary
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float4 worldPosition : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 uv : TEXCOORD;
    float clip : SV_ClipDistance0;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

float3 expandNormals(float3 normalTexture)
{
    float3 normal = normalTexture.rgb;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    return normalize(normal);
}

float3 CompressToRGB(float3 value)
{
    return 0.5f * (value + 1.0f);
}

float3 SaturateColor(float3 aColor, float aSaturation)
{
    float3 gray = dot(aColor, float3(0.3f, 0.3f, 0.3f));
    float3 saturate = lerp(gray, aColor, aSaturation);
    return saturate;
}

PixelOutput main(PixelInput input)
{
    //GET DATA---------------------------------------------------------------------------------
    
    //Get textures
    float3 sandColorSample = sandTexture.Sample(samplerState, input.uv);
    float3 sandColor = SaturateColor(sandColorSample, 1.0f);
    float3 sandMat = sandMaterial.Sample(samplerState, input.uv);
    float3 sandNormalToExpand = sandNormalMap.Sample(samplerState, input.uv);
    float3 sandNormal = expandNormals(sandNormalToExpand);
    
    float3 grassColorSample = grassTexture.Sample(samplerState, input.uv);
    float3 grassColor = SaturateColor(grassColorSample, 1.0f);
    float3 grassMat = grassMaterial.Sample(samplerState, input.uv);
    float3 grassNormalToExpand = grassNormalMap.Sample(samplerState, input.uv);
    float3 grassNormal = expandNormals(grassNormalToExpand);
    
    float3 rockColor = rockTexture.Sample(samplerState, input.uv);
    float3 rockMat = rockMaterial.Sample(samplerState, input.uv);
    float3 rockNormalToExpand = rockNormalMap.Sample(samplerState, input.uv);
    float3 rockNormal = expandNormals(rockNormalToExpand);
    
    // Slope and height blend
    float slopeBlend = smoothstep(slopeBlendMin, slopeBlendMax, input.normal.y);
    float heightBlend = smoothstep(heightBlendMin, heightBlendMax, input.worldPosition.y);
    slopeBlend *= blendStrength;
    heightBlend *= blendStrength;
    
    // Blend color, material and normal textures
    float3 blendedColor = lerp(rockColor, lerp(sandColor, grassColor, heightBlend), slopeBlend);
    float3 blendedMaterial = lerp(rockMat, lerp(sandMat, grassMat, heightBlend), slopeBlend);
    float3 normal = lerp(rockNormal, lerp(sandNormal, grassNormal, heightBlend), slopeBlend);
    
    // Extract material properties
    float ambientOcclusion = blendedMaterial.r;
    float roughness = blendedMaterial.g;
    float metalness = blendedMaterial.b;
    
    // CALCULATE LIGHT-----------------------------------------------------------------------------------

    // Build tangent to world transformation matrix
    float3x3 tangentToWorld = float3x3
    (
    normalize(input.tangent),
    normalize(input.bitangent),
    normalize(input.normal)
    );

    // Get view direction in world space
    float3 viewDirWorld = normalize(cameraPosition.xyz - input.worldPosition.xyz);

    // Transform normal from tangent space to world space
    float3 normalWorld = mul(normal, tangentToWorld);
    normalWorld = normalize(normalWorld);

    // Compute colors (using the world space vectors)
    float3 diffuseColor = lerp((float3) 0.0f, blendedColor.rgb, 1.0f - metalness);
    float3 specularColor = lerp((float3) 0.0f, blendedColor.rgb, metalness);

    // Ambient (all vectors in world space)
    float3 ambience = EvaluateAmbiance
    (
    environmentTexture, normalWorld, input.normal,
    viewDirWorld, roughness,
    ambientOcclusion, diffuseColor, specularColor
    );
    
    ambience *= (ambientLightColorAndIntensity.xyz * ambientLightColorAndIntensity.w);

    // Directional (all vectors in world space)
    float3 lightDir = normalize(directionalLightDirection);
    float3 directionalLight = EvaluateDirectionalLight(
    diffuseColor, specularColor, normalWorld, roughness,
    directionalLightColorAndIntensity.xyz, lightDir, viewDirWorld
    );
    directionalLight *= directionalLightColorAndIntensity.w;

    // OUTPUT -------------------------------------------------------------------------------------
    PixelOutput result;
    float3 colorRgb = ambience + directionalLight;
    result.color = float4(tonemap_s_gamut3_cine(colorRgb), 1.0f);
    return result;
}
