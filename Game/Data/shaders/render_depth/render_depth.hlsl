#include <shader_common_vs.hlsli>
#include <shader_common_ps.hlsli>

// Vertex/Pixel inputs
struct VertexInput {
	float3 position : POSITION0;
	float3 normal   : NORMAL0;
	float2 texCoord : TEXCOORD0;
};

struct PixelInput {
	float4 svpos : SV_POSITION;
	float3 position : POSITION0;
	float3 normal : NORMAL0;
	float2 texCoord : TEXCOORD0;
};

// Resources
Texture2D DepthMap : register(TEX_REGISTER_USER0);
SamplerState Sampler : register(SAM_REGISTER_USER0);

// Vertex shader
PixelInput vsMain(VertexInput input)
{
	float4 pos = float4(input.position, 1.0f);

	PixelInput output;

	output.svpos = mul(mul(viewProj, world), pos);
	output.position = mul(world, pos).xyz;

	float4 normal = float4(normalize(input.normal), 0.0f);
	output.normal = mul(normal, worldInverseTranspose).xyz;

	output.texCoord = input.texCoord;

	return output;
}

// Pixel Shader
float4 psMain(PixelInput input) : SV_TARGET
{
	float2 texCoord = float2(1.0f, 1.0f) - input.texCoord;
	float4 texColor = DepthMap.Sample(Sampler, texCoord);

	float depth = 1.0f - (texColor.x / 2);
	return float4(depth, depth, depth, 1.0f);
}