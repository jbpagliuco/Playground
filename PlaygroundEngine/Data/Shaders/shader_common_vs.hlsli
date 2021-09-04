#if !defined(SHADER_COMMON_VS_HLSLI)
#define SHADER_COMMON_VS_HLSLI

#include <shader_common.hlsli>

void SetCommonPixelInputValues(inout CommonPixelInput output, inout CommonVertexInput input)
{
	float4 pos = float4(input.position, 1.0f);

	output.svpos = mul(mul(viewProj, world), pos);
	output.position = mul(world, pos).xyz;

	float4 normal = float4(normalize(input.normal), 0.0f);
	output.normal = mul(normal, worldInverseTranspose).xyz;

	output.texCoord = input.texCoord;

	for (int i = 0; i < numShadowCastingLights; ++i) {
		output.shadowProjTexCoords[i] = mul(mul(lightViewProj[i], world), pos);
	}
}

#endif // defined(SHADER_COMMON_VS_HLSLI)