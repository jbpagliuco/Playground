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
	float2 texCoord : TEXCOORD0;
};

// Vertex shader
PixelInput vsMain(VertexInput input)
{
	float4 pos = float4(input.position, 1.0f);

	PixelInput output;
	output.svpos = mul(mul(viewProj, world), pos);
	output.texCoord = input.texCoord;

	return output;
}

// Pixel Shader
float4 psMain(PixelInput input) : SV_TARGET
{
	return float4(input.texCoord.x, input.texCoord.y, 0.0f, 1.0f);
}