#include <shader_common_vs.hlsli>
#include <shader_common_ps.hlsli>

// Vertex/Pixel inputs
struct VertexInput : CommonVertexInput
{
};

struct PixelInput : CommonPixelInput
{
};

// Material data
cbuffer cbMaterial : register(CB_REGISTER_MATERIAL)
{
	float4 matDiffuse;
	float4 matSpecular;
};

// Resources
Texture2D DiffuseTexture : register(TEX_REGISTER_USER0);
SamplerState Sampler : register(SAM_REGISTER_USER0);

// Vertex shader
PixelInput vsMain(VertexInput input)
{
	PixelInput output = (PixelInput)0;
	SetCommonPixelInputValues((CommonPixelInput)output, (CommonVertexInput)input);

	return output;
}

// Pixel Shader
float4 psMain(PixelInput input) : SV_TARGET
{
	LightingResult lit = ComputeFullLighting(input, matSpecular.w);

	float2 texCoord = float2(1.0f, 1.0f) - input.texCoord;
	float4 texColor = DiffuseTexture.Sample(Sampler, texCoord);

	float4 ambient = texColor * matDiffuse * globalAmbient;
	float4 diffuse = texColor * matDiffuse * lit.diffuse;
	float4 specular = float4(matSpecular.xyz * lit.specular.xyz, 1.0f);

	return ambient + diffuse + specular;
}