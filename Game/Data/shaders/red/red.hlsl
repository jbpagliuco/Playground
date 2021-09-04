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

// Vertex shader
PixelInput vsMain(VertexInput input)
{
	PixelInput output = (PixelInput)0;
	SetCommonPixelInputValues((CommonPixelInput)output, (CommonVertexInput)input);

	return output;
}

// Pixel shader
float4 psMain(PixelInput input) : SV_TARGET
{
	LightingResult lit = ComputeFullLighting(input, matSpecular.w);

	float4 ambient = matDiffuse * globalAmbient;
	float4 diffuse = matDiffuse * lit.diffuse;
	float4 specular = float4(matSpecular.xyz * lit.specular.xyz, 1.0f);

	return ambient + diffuse + specular;
}