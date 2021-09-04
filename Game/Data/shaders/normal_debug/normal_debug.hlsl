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
	float3 normal : NORMAL0;
};

// Vertex shader
PixelInput vsMain(VertexInput input)
{
	float4 pos = float4(input.position, 1.0f);

	PixelInput output;
	output.svpos = mul(mul(viewProj, world), pos);

	float4 normal = float4(normalize(input.normal), 0.0f);
	output.normal = mul(normal, worldInverseTranspose).xyz;

	return output;
}

// Pixel Shader
float4 psMain(PixelInput input) : SV_TARGET
{
	return float4(normalize(input.normal), 1.0f);
}