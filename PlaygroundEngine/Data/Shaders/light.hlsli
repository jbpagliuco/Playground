#if !defined(LIGHT_HLSLI)
#define LIGHT_HLSLI

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct Light
{
	int type;
	bool enabled;
	float intensity;
	int shadowCastingIndex;

	float3 position;
	float _padding1;

	float3 direction;
	float radius;

	float4 color;

	float spotLightAngle;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};


struct LightingResult
{
	float4 diffuse;
	float4 specular;
};

float4 ComputeDiffuse(Light light, float3 L, float3 N)
{
	const float NdotL = max(dot(N, L), 0.0f);
	return light.color * NdotL;
}

float4 ComputeSpecular(Light light, float specularExp, float3 V, float3 L, float3 N)
{
	const float3 R = normalize(reflect(-L, N));
	const float RdotV = max(dot(R, V), 0.0f);

	return light.color * pow(RdotV, specularExp);
}

float ComputeAttenuation(Light light, float d)
{
	const float att = light.constantAttenuation + (light.linearAttenuation * d) + (light.quadraticAttenuation * d * d);
	return 1.0f / att;
}

LightingResult ComputeDirectionalLight(Light light, float specularExp, float3 V, float3 P, float3 N)
{
	LightingResult result;

	const float3 L = -light.direction;

	result.diffuse = ComputeDiffuse(light, L, N);
	result.specular = ComputeSpecular(light, specularExp, V, L, N);

	return result;
}

LightingResult ComputePointLight(Light light, float specularExp, float3 V, float3 P, float3 N)
{
	LightingResult result;
	result.diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	result.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 L = light.position - P;
	float distance = length(L);

	if (distance > light.radius) {
		return result;
	}

	L = L / distance;

	float attenuation = ComputeAttenuation(light, distance);

	result.diffuse = ComputeDiffuse(light, L, N) * attenuation;
	result.specular = ComputeSpecular(light, specularExp, V, L, N) * attenuation;

	return result;
}

float ComputeSpotIntensity(Light light, float3 L)
{
	const float minCos = cos(light.spotLightAngle);
	const float maxCos = (minCos + 1.0f) / 2.0f;
	const float cosAngle = dot(light.direction, -L);
	return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult ComputeSpotLight(Light light, float specularExp, float3 V, float3 P, float3 N)
{
	LightingResult result;
	result.diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	result.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 L = light.position - P;
	float distance = length(L);

	if (distance > light.radius) {
		//return result;
	}

	L = L / distance;

	float spotIntensity = ComputeSpotIntensity(light, L);
	float attenuation = ComputeAttenuation(light, distance);

	result.diffuse = ComputeDiffuse(light, L, N) * attenuation * spotIntensity;
	result.specular = ComputeSpecular(light, specularExp, V, L, N) * attenuation * spotIntensity;
	
	return result;
}

LightingResult ComputeLighting(Light light, float specularExp, float3 V, float3 P, float3 N)
{
	LightingResult result;
	result.diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	result.specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (!light.enabled) {
		return result;
	}

	switch (light.type) {
	case LIGHT_TYPE_DIRECTIONAL:
		result = ComputeDirectionalLight(light, specularExp, V, P, N);
		break;

	case LIGHT_TYPE_POINT:
		result = ComputePointLight(light, specularExp, V, P, N);
		break;

	case LIGHT_TYPE_SPOT:
		result = ComputeSpotLight(light, specularExp, V, P, N);
		break;
	}

	result.diffuse *= light.intensity;
	result.specular *= light.intensity;

	return result;
}

#endif // defined(LIGHT_HLSLI)