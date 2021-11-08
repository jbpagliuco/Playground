#if !defined(SHADER_COMMON_HLSLI)
#define SHADER_COMMON_HLSLI

#include <shader_constants.h>
#include <light.hlsli>

// Constant Buffers
cbuffer cbPerFrame : register(CB_REGISTER_PER_FRAME)
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

// Engine textures
Texture2DArray shadowMaps : register(TEX_REGISTER_SHADOWMAP);

// Sampler states
SamplerState samplerPointWrap : register(SAM_REGISTER_POINT_WRAP);
SamplerState samplerPointClamp : register(SAM_REGISTER_POINT_CLAMP);
SamplerState samplerLinearWrap : register(SAM_REGISTER_LINEAR_WRAP);
SamplerState samplerLinearClamp : register(SAM_REGISTER_LINEAR_CLAMP);
SamplerState samplerAnisotropicWrap : register(SAM_REGISTER_ANISOTROPIC_WRAP);
SamplerState samplerAnisotropicClamp : register(SAM_REGISTER_ANISOTROPIC_CLAMP);

#define SAMPLER_SHADOW_MAP samplerPointClamp

// Common vertex input struct
struct CommonVertexInput
{
	float3 position : POSITION0;
	float3 normal   : NORMAL0;
	float2 texCoord : TEXCOORD0;
};

// Common pixel input struct
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