cbuffer cbPerFrame : register(b0)
{
	matrix cameraViewProj;
};

TextureCube cubeMap;
SamplerState cubeMapSampler;

// Vertex/Pixel inputs
struct VertexInput
{
	float3 position : POSITION0;
};

struct PixelInput
{
	float4 svpos : SV_POSITION;
	float3 texCoord : TEXCOORD0;
};

// Vertex shader
PixelInput vsMain(VertexInput input)
{
	PixelInput output;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane)
	output.svpos = mul(cameraViewProj, float4(input.position, 1.0f)).xyww;

	// use local vertex position as cube map lookup vector
	output.texCoord = input.position;

	return output;
}

// Pixel shader
float4 psMain(PixelInput input) : SV_TARGET
{
	return cubeMap.Sample(cubeMapSampler, input.texCoord);
}