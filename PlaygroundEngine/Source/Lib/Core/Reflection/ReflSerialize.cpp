#include "ReflSerialize.h"

#include "Debug/Assert.h"
#include "Reflection/ReflMarkup.h"
#include "Streaming/Stream.h"
#include "Util/Color.h"

namespace playground
{
	template <typename ReflectedType, typename ValueType>
	static void WriteReflectedValue(void* obj, const ValueType& value)
	{
		*(static_cast<ReflectedType*>(obj)) = static_cast<ReflectedType>(value);
	}

	static void ReflectionDeserializePOD(const refl::TypeInfo& typeInfo, void* obj, const DeserializationParameterMap& value)
	{
		switch (typeInfo.mDataType) {
		case refl::DataType::BOOL:
			WriteReflectedValue<bool>(obj, value.AsBool());
			return;

		case refl::DataType::UINT8:
			WriteReflectedValue<uint8_t>(obj, value.AsInt());
			return;

		case refl::DataType::UINT16:
			WriteReflectedValue<uint16_t>(obj, value.AsInt());
			return;

		case refl::DataType::UINT32:
			WriteReflectedValue<uint32_t>(obj, value.AsInt());
			return;

		case refl::DataType::UINT64:
			WriteReflectedValue<uint64_t>(obj, value.AsInt());
			return;

		case refl::DataType::INT8:
			WriteReflectedValue<int8_t>(obj, value.AsInt());
			return;

		case refl::DataType::INT16:
			WriteReflectedValue<int16_t>(obj, value.AsInt());
			return;

		case refl::DataType::INT32:
			WriteReflectedValue<int32_t>(obj, value.AsInt());
			return;

		case refl::DataType::INT64:
			WriteReflectedValue<bool>(obj, value.AsInt());
			return;

		case refl::DataType::FLOAT:
			WriteReflectedValue<float>(obj, value.AsFloat());
			return;

		case refl::DataType::DOUBLE:
			WriteReflectedValue<double>(obj, value.AsFloat());
			return;

		case refl::DataType::LONG_DOUBLE:
			WriteReflectedValue<long double>(obj, value.AsFloat());
			return;
		};

		CORE_ASSERT(false, "Unhandled POD type.");
	}

	static void ReflectionDeserializeEnum(const refl::Enum& reflEnum, void* obj, const DeserializationParameterMap& parameters)
	{
		int8_t value;
		const bool found = reflEnum.GetValue(value, parameters.value);
		CORE_ASSERT_RETURN(found, "Failed to find value '%s' within enum '%s'.", parameters.value, reflEnum.mQualifiedName.c_str());

		WriteReflectedValue<int8_t>(obj, value);
	}

	void ReflectionDeserialize(const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters)
	{
		// Special classes
		if (reflClass.mName == "AssetID") {
			// Request the asset
			const AssetID requestedAssetID = RequestAsset(parameters.AsFilepath());
			WriteReflectedValue<decltype(AssetID::mID)>(obj, requestedAssetID);
			return;
		}
		else if (reflClass.mName == "ColorF") {
			// Deserialize this color from a string
			if (parameters.value != "") {
				WriteReflectedValue<ColorF>(obj, ColorF(parameters.value));
			}
		}

		// Deserialize each field.
		for (const auto& reflField : reflClass.mFields) {
			// Gets the starting address of this field.
			void* fieldStart = reflField.GetRawDataPtr(obj);

			// Does the parameter map contain this field?
			const std::string fieldName = reflField.HasAttribute(REFL_ATTR_NAME) ? reflField.GetAttribute(REFL_ATTR_NAME) : reflField.mName;
			if (!parameters.HasChild(fieldName)) {
				continue;
			}
			const DeserializationParameterMap& fieldValue = parameters[fieldName];

			// Class type?
			if (reflField.mTypeInfo.IsClass()) {
				ReflectionDeserialize(*reflField.GetClass(), fieldStart, fieldValue);
			}
			// Enum type?
			else if (reflField.mTypeInfo.IsEnum()) {
				ReflectionDeserializeEnum(*reflField.GetEnum(), fieldStart, fieldValue);
			}
			// POD type?
			else if (reflField.mTypeInfo.IsPOD()) {
				ReflectionDeserializePOD(reflField.mTypeInfo, fieldStart, fieldValue);
			}
			else {
				CORE_ASSERT(false, "Unrecognized reflection data type.");
			}
		}
	}
}