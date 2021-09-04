cbuffer cbPerFrame : register(b0)
{
	matrix lightVP;
};

cbuffer cbPerObject : register(b1)
{
	matrix world;
};

// Vertex/Pixel inputs
struct VertexInput
{
	float3 position : POSITION0;
};

struct PixelInput
{
	float4 svpos : SV_POSITION;
};

// Vertex shader
PixelInput vsMain(VertexInput input)
{
	PixelInput output;

	output.svpos = mul(mul(lightVP, world), float4(input.position, 1.0f));

	return output;
}

// Pixel shader
void psMain(PixelInput input)
{
}