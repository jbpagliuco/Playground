#include "NGA/NGACommon.h"

#include <map>
#include <string>

#include "Core/Util/Util.h"

namespace playground
{
	static std::string SemanticStrings[] = {
		"POSITION",
		"NORMAL",
		"TEXCOORD",
		"BINORMAL",
		"TANGENT",
		"COLOR",

		"UNKNOWN"
	};

	NGAFormat GetFormatFromString(const char *s)
	{
		static std::map<std::string, NGAFormat> CONV = {
			{ "float", NGAFormat::R32_FLOAT },
			{ "float2", NGAFormat::R32G32_FLOAT },
			{ "float3", NGAFormat::R32G32B32_FLOAT },
			{ "float4", NGAFormat::R32G32B32A32_FLOAT },
			{ "color", NGAFormat::R32G32B32A32_FLOAT }
		};

		return CONV[s];
	}

	size_t GetFormatByteSize(NGAFormat format)
	{
		static constexpr size_t SIZES[] = {
			4,
			8,
			12,
			16
		};

		return SIZES[(int)format];
	}

	const char* GetSemanticName(NGAVertexSemanticType type)
	{
		return SemanticStrings[(int)type].c_str();
	}
	
	NGAVertexSemanticType GetSemanticType(const char *name)
	{
		for (int i = 0; i < STATIC_ARRAY_SIZE(SemanticStrings); ++i) {
			if (name == SemanticStrings[i]) {
				return (NGAVertexSemanticType)i;
			}
		}

		CORE_ASSERT(false, "Failed to find semantic type with name '%s'", name);
		return NGAVertexSemanticType::UNKNOWN;
	}

	bool NGAFormatIsTypeless(NGAFormat format)
	{
		return format >= NGAFormat::R24G8_TYPELESS && format <= NGAFormat::R32G32B32A32_TYPELESS;
	}
}