#include <shader_common_vs.hlsli>
#include <shader_common_ps.hlsli>

// Vertex/Pixel inputs
struct VertexInput : CommonVertexInput
{
	float3 tangent : TANGENT0;
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
SamplerState DiffuseSampler : register(SAM_REGISTER_USER0);

Texture2D NormalTexture : register(TEX_REGISTER_USER1);
SamplerState NormalSampler : register(SAM_REGISTER_USER1);

PixelInput vsMain(VertexInput input)
{
	PixelInput output = (PixelInput)0;
	SetCommonPixelInputValues((CommonPixelInput)output, (CommonVertexInput)input);

	float4 tangent = float4(normalize(input.tangent), 0.0f);
	output.tangent = mul(worldInverseTranspose, tangent).xyz;

	return output;
}

float4 psMain(PixelInput input) : SV_TARGET
{
	float2 texCoord = float2(1.0f, 1.0f) - input.texCoord;

	float3 N = normalize(input.normal);
	float3 T = normalize(input.tangent - dot(input.tangent, N) * N);
	float3 B = cross(N, T);
	float3 normal = ComputeNormalFromMap(NormalTexture.Sample(NormalSampler, texCoord), T, B, N);

	LightingResult lit = ComputeFullLighting(input, matSpecular.w, normal);

	float4 texColor = DiffuseTexture.Sample(DiffuseSampler, texCoord);

	float4 ambient = texColor * matDiffuse * globalAmbient;
	float4 diffuse = texColor * matDiffuse * lit.diffuse;
	float4 specular = float4(matSpecular.xyz * lit.specular.xyz, 1.0f);

	return ambient + diffuse + specular;
}