#include "Common.hlsli"
#include "PBRFunctions.hlsli"

Texture2D colorTexture : register(t12);
Texture2D materialTexture : register(t13);
Texture2D normalTexture : register(t14);

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

float3 expandNormals(float4 normalTexture)
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
    PixelOutput result;
    
    // GET DATA ---------------------------------------------------------------------------------

    // Get textures
    float4 colorSample = colorTexture.Sample(samplerState, input.uv).rgba;
    float3 color = SaturateColor(colorSample.rgb, 1.0f);
    float3 material = materialTexture.Sample(samplerState, input.uv);
    float4 normalToExpand = normalTexture.Sample(samplerState, input.uv);
    float3 normal = expandNormals(normalToExpand);

    //alpha treshold
    float alpha = colorSample.a;
    if (alpha <= 0.9f)
    {
        discard;
        result.color = float4(0.f, 0.f, 0.f, 0.f);
        return result;
    }
    
    // Extract material properties
        float ambientOcclusion = material.r;
    float roughness = material.g;
    float metalness = material.b;
    
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
    float3 diffuseColor = lerp((float3) 0.0f, color.rgb, 1.0f - metalness);
    float3 specularColor = lerp((float3) 0.0f, color.rgb, metalness);

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
    float3 colorRgb = ambience + directionalLight;
    
    result.color = float4(tonemap_s_gamut3_cine(colorRgb), alpha);
    return result;
}
