#pragma once

#include <stdint.h>
#include <vector>

#include "NGA/NGADefs.h"

#if CORE_RENDER_API(DX11)
#include <d3d11.h>
#endif

#include "Core/Debug/Assert.h"
#include "Core/Reflection/ReflMarkup.h"
#include "Core/Util/BitUtil.h"

#include "NGA/NGAPipelineStateDefs.h"

#include "NGACommon.reflgen.h"


#define NGA_GPU_CLASS(ClassName)								\
	public:														\
	ClassName() {}												\
	~ClassName() { Destruct(); }								\
	ClassName(const ClassName&) = delete;						\
	ClassName& operator=(const ClassName&) = delete;			\
	static const ClassName INVALID;								\
	private:

#define NGA_GPU_CLASS_IMPLEMENT(ClassName)						\
	const ClassName ClassName::INVALID

namespace playground
{
	enum class REFLECTED NGAFormat : uint8_t
	{
		// Color formats
		R32_FLOAT					REFL_NAME("float") = 0,
		R32G32_FLOAT				REFL_NAME("float2"),
		R32G32B32_FLOAT				REFL_NAME("float3"),
		R32G32B32A32_FLOAT			REFL_NAME("float4"),

		R16_FLOAT,
		R16G16_FLOAT,
		R16G16B16A16_FLOAT,
		
		BC1_UNORM,
		B8G8R8A8_UNORM,
		
		// Depth formats
		D16_UNORM,
		D24_UNORM_S8_UINT,
		D32_FLOAT,

		// Typeless formats
		R24G8_TYPELESS,
		R32_TYPELESS,
		R32G32_TYPELESS,
		R32G32B32_TYPELESS,
		R32G32B32A32_TYPELESS,

		UNKNOWN,

		SIZE
	};

	enum class REFLECTED NGAUsage : uint8_t
	{
		IMMUTABLE = 0,
		GPU_WRITE,
		CPU_WRITE,
		CPU_READ_WRITE
	};



	enum NGAShaderStage
	{
		NGA_SHADER_STAGE_VERTEX		= 1 << 0,
		NGA_SHADER_STAGE_PIXEL		= 1 << 1,
		NGA_SHADER_STAGE_ALL		= 0x3 // Keep up to date with above
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(NGAShaderStage);



	enum NGATextureBindFlags
	{
		NGA_TEXTURE_BIND_NONE				= 0,
		NGA_TEXTURE_BIND_SHADER_RESOURCE	= 1 << 0,
		NGA_TEXTURE_BIND_RENDER_TARGET		= 1 << 1,
		NGA_TEXTURE_BIND_DEPTH_STENCIL		= 1 << 2
	};
	CORE_DEFINE_ENUM_FLAG_OPERATORS(NGATextureBindFlags);

	enum class REFLECTED NGATextureFilter : uint8_t
	{
#if CORE_RENDER_API(DX11)
		MIN_MAG_MIP_POINT = D3D11_FILTER_MIN_MAG_MIP_POINT,
		MIN_MAG_POINT_MIP_LINEAR = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
		MIN_POINT_MAG_LINEAR_MIP_POINT = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
		MIN_LINEAR_MAG_MIP_POINT = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,

		MIN_MAG_MIP_LINEAR = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		MIN_MAG_LINEAR_MIP_POINT = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		MIN_LINEAR_MAG_POINT_MIP_LINEAR = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		MIN_POINT_MAG_MIP_LINEAR = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,

		ANISOTROPIC = D3D11_FILTER_ANISOTROPIC
#endif
	};

	enum class REFLECTED NGATextureAddressMode : uint8_t
	{
#if CORE_RENDER_API(DX11)
		WRAP = D3D11_TEXTURE_ADDRESS_WRAP,
		MIRROR = D3D11_TEXTURE_ADDRESS_MIRROR,
		CLAMP = D3D11_TEXTURE_ADDRESS_CLAMP,
		BORDER = D3D11_TEXTURE_ADDRESS_BORDER,
		MIRROR_ONCE = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
#endif
	};




	enum class REFLECTED NGAVertexSemanticType : uint8_t
	{
		POSITION,
		NORMAL,
		TEXCOORD,
		BINORMAL,
		TANGENT,
		COLOR,

		UNKNOWN
	};

	struct REFLECTED NGAVertexAttribute
	{
		GENERATED_REFLECTION_CODE();

	public:
		NGAFormat mFormat						REFLECTED;
		NGAVertexSemanticType mSemanticType		REFLECTED;
		int mSemanticIndex						REFLECTED;
		int mOffset								REFLECTED;
	};

	struct REFLECTED NGAVertexFormatDesc
	{
		GENERATED_REFLECTION_CODE();

	public:
		std::vector<NGAVertexAttribute> mAttributes REFLECTED;
	};

	/////////////////////////////////////////////////////////////////////////


	NGAFormat GetFormatFromString(const char *s);
	size_t GetFormatByteSize(NGAFormat format);

	const char* GetSemanticName(NGAVertexSemanticType type);
	NGAVertexSemanticType GetSemanticType(const char *name);

	bool NGAFormatIsTypeless(NGAFormat format);
}