#if !defined(SHADER_COMMON_PS_HLSLI)
#define SHADER_COMMON_PS_HLSLI

#include <shader_common.hlsli>

// Constants
static const float SHADOW_EPSILON = 0.001f;
static const float SMAP_SIZE = (float)SHADOWMAP_SIZE;
static const float SMAP_DX = 1.0f / SMAP_SIZE;
static const float SHADOW_BIAS = 0.0f;

// Functions
float ComputeShadowFactor(float4 projTexCoord, int shadowMapIndex)
{
	// Complete the projection
	projTexCoord.xyz /= projTexCoord.w;

	// Points outside the light volume are in shadow
	if (projTexCoord.x < -1.0f || projTexCoord.x > 1.0f ||
		projTexCoord.y < -1.0f || projTexCoord.y > 1.0f ||
		projTexCoord.z < 0.0f)
	{
		return 0.0f;
	}

	// Transform from NDC space to texture space
	projTexCoord.x = +0.5f * projTexCoord.x + 0.5f;
	projTexCoord.y = -0.5f * projTexCoord.y + 0.5f;

	// Depth in NDC space
	float depth = projTexCoord.z - SHADOW_BIAS;

	// Sample shadow map to get nearest depth to light
	float s0 = shadowMaps.Sample(shadowMapSampler, float3(projTexCoord.xy, shadowMapIndex)).r;
	float s1 = shadowMaps.Sample(shadowMapSampler, float3(projTexCoord.xy + float2(SMAP_DX, 0.0f), shadowMapIndex)).r;
	float s2 = shadowMaps.Sample(shadowMapSampler, float3(projTexCoord.xy + float2(0.0f, SMAP_DX), shadowMapIndex)).r;
	float s3 = shadowMaps.Sample(shadowMapSampler, float3(projTexCoord.xy + float2(SMAP_DX, SMAP_DX), shadowMapIndex)).r;

	float result0 = depth <= s0 + SHADOW_EPSILON;
	float result1 = depth <= s1 + SHADOW_EPSILON;
	float result2 = depth <= s2 + SHADOW_EPSILON;
	float result3 = depth <= s3 + SHADOW_EPSILON;

	// Transform to texel space
	float2 texelPos = SMAP_SIZE * projTexCoord.xy;

	// Determine the interpolation amounts
	float2 t = frac(texelPos);

	return lerp(lerp(result0, result1, t.x),
				lerp(result2, result3, t.x), t.y);
}

LightingResult ComputeFullLighting(CommonPixelInput pixelInput, float specularExp, float3 normal)
{
	LightingResult lit;
	lit.diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	lit.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 P = pixelInput.position;
	float3 V = normalize(eyePosition - P).xyz;

	for (int i = 0; i < numLights; ++i) {
		LightingResult result = ComputeLighting(lights[i], specularExp, V, P, normal);
		float4 diffuse = result.diffuse;
		float4 specular = result.specular;

		// Apply shadowing?
		int shadowCasterIndex = lights[i].shadowCastingIndex;
		if (shadowCasterIndex != -1) {
			float4 projectedTexCoord = pixelInput.shadowProjTexCoords[shadowCasterIndex];
			float shadowFactor = ComputeShadowFactor(projectedTexCoord, shadowCasterIndex);

			diffuse *= shadowFactor;
			specular *= shadowFactor;
		}

		lit.diffuse += diffuse;
		lit.specular += specular;
	}

	lit.diffuse = saturate(lit.diffuse);
	lit.specular = saturate(lit.specular);

	return lit;
}

LightingResult ComputeFullLighting(CommonPixelInput pixelInput, float specularExp)
{
	float3 normal = normalize(pixelInput.normal);
	return ComputeFullLighting(pixelInput, specularExp, normal);
}

float3 ComputeNormalFromMap(float4 normalMapTexel, float3 T, float3 B, float3 N)
{
	normalMapTexel = (normalMapTexel * 2.0f) - 1.0f;
	
	float3x3 TBN = float3x3(T, B, N);
	float3 bumpNormal = mul(normalMapTexel.xyz, TBN);
	return normalize(bumpNormal);
}

#endif // defined(SHADER_COMMON_PS_HLSLI)