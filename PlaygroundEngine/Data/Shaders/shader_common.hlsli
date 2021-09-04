#if !defined(SHADER_COMMON_HLSLI)
#define SHADER_COMMON_HLSLI

#include <shader_constants.h>
#include <light.hlsli>

// Constant Buffers
cbuffer cbCamera : register(CB_REGISTER_PER_FRAME)
{
	matrix viewProj;
	matrix lightViewProj[MAX_SHADOWMAPS];
	int numShadowCastingLights;
};

cbuffer cbPerObject : register(CB_REGISTER_PER_OBJECT)
{
	matrix world;
	matrix worldInverseTranspose;
};

cbuffer cbLights : register(CB_REGISTER_LIGHTS)
{
	float4 globalAmbient;
	float3 eyePosition;
	int numLights;
	Light lights[MAX_LIGHTS];
};

Texture2DArray shadowMaps : register(TEX_REGISTER_SHADOWMAP);
SamplerState shadowMapSampler : register(SAM_REGISTER_SHADOWMAP);

struct CommonVertexInput
{
	float3 position : POSITION0;
	float3 normal   : NORMAL0;
	float2 texCoord : TEXCOORD0;
};

struct CommonPixelInput
{
	float4 svpos : SV_POSITION;
	float3 position : POSITION0;
	
	float3 normal : NORMAL0;
	float3 tangent : TANGENT0;

	float2 texCoord : TEXCOORD0;

	float4 shadowProjTexCoords[MAX_SHADOWMAPS] : TEXCOORD1;
};

#endif // defined(SHADER_COMMON_HLSLI)