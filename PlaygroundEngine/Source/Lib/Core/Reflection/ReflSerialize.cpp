#include "ReflSerialize.h"

#include "Debug/Assert.h"
#include "Reflection/ReflMarkup.h"
#include "Streaming/Stream.h"
#include "Util/Color.h"
#include "Util/String.h"

namespace playground
{
	static void ReflectionDeserializeField(const refl::Class& reflClass, const refl::Field& reflField, void* obj, const DeserializationParameterMap& parameters);
	static void ReflectionDeserializeClass(const std::string& className, const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters);

	template <typename ReflectedType, typename ValueType>
	static void WriteReflectedValue(void* obj, const ValueType& value)
	{
		*(static_cast<ReflectedType*>(obj)) = static_cast<ReflectedType>(value);
	}

	static void ReflectionDeserializePOD(const refl::TypeInfo& typeInfo, void* obj, const DeserializationParameterMap& value, const std::string& multiplierVal)
	{
		const float floatMultiplier = (multiplierVal != "") ? atof(multiplierVal.c_str()) : 1.0f;
		const int intMultiplier = (multiplierVal != "") ? atoi(multiplierVal.c_str()) : 1;

		switch (typeInfo.mDataType) {
		case refl::DataType::BOOL:
			WriteReflectedValue<bool>(obj, value.AsBool());
			return;

		case refl::DataType::UINT8:
			WriteReflectedValue<uint8_t>(obj, value.AsInt() * intMultiplier);
			return;

		case refl::DataType::UINT16:
			WriteReflectedValue<uint16_t>(obj, value.AsInt() * intMultiplier);
			return;

		case refl::DataType::UINT32:
			WriteReflectedValue<uint32_t>(obj, value.AsInt() * intMultiplier);
			return;

		case refl::DataType::UINT64:
			WriteReflectedValue<uint64_t>(obj, value.AsInt() * intMultiplier);
			return;

		case refl::DataType::INT8:
			WriteReflectedValue<int8_t>(obj, value.AsInt() * intMultiplier);
			return;

		case refl::DataType::INT16:
			WriteReflectedValue<int16_t>(obj, value.AsInt() * intMultiplier);
			return;

		case refl::DataType::INT32:
			WriteReflectedValue<int32_t>(obj, value.AsInt() * intMultiplier);
			return;

		case refl::DataType::INT64:
			WriteReflectedValue<bool>(obj, value.AsInt() * intMultiplier);
			return;

		case refl::DataType::FLOAT:
			WriteReflectedValue<float>(obj, value.AsFloat() * floatMultiplier);
			return;

		case refl::DataType::DOUBLE:
			WriteReflectedValue<double>(obj, value.AsFloat() * floatMultiplier);
			return;

		case refl::DataType::LONG_DOUBLE:
			WriteReflectedValue<long double>(obj, value.AsFloat() * floatMultiplier);
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

	static bool MaybeDeserializeReflectedClass(const std::string className, const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters)
	{
		// std library classes
		if (className == "std::string") {
			WriteReflectedValue<std::string>(obj, parameters.value);
			return true;
		}

		// playground classes
		if (reflClass == AssetID::StaticClass()) {
			// Request the asset
			const AssetID requestedAssetID = RequestAsset(parameters.AsFilepath());
			WriteReflectedValue<decltype(AssetID::mID)>(obj, requestedAssetID);
			return true;
		}
		else if (reflClass == ColorF::StaticClass()) {
			// Deserialize this color from a string
			if (parameters.value != "") {
				WriteReflectedValue<ColorF>(obj, ColorF(parameters.value));
				return true;
			}
		}

		// Not handled
		return false;
	}

	static bool ShouldDoEnumMatch(const refl::Class& reflClass, const refl::Field& reflField, void* obj, const DeserializationParameterMap& parameters)
	{
		// Get the enum value
		const refl::Field* enumField = reflClass.GetField(reflField.GetAttribute(REFL_ATTR_ENUM_MATCH_VAR));
		const refl::Enum* enumClass = enumField->GetEnum();
		const int8_t enumValue = *enumField->GetDataPtr<int8_t>(obj);

		// See if this matches
		const std::string enumMatches = reflField.GetAttribute(REFL_ATTR_ENUM_MATCH_MATCHES);
		for (auto& enumMatch : StrSplit(enumMatches, ",")) {
			StrTrim(enumMatch);
			if (enumClass->GetValueString(enumValue) == enumMatch) {
				return true;
			}
		}

		return false;
	}

	static void ReflectionDeserializeField(const refl::Class& reflClass, const refl::Field& reflField, void* obj, const DeserializationParameterMap& parameters)
	{
		// Get reflected name
		std::string fieldName = reflField.HasAttribute(REFL_ATTR_NAME) ? reflField.GetAttribute(REFL_ATTR_NAME) : reflField.mName;
		if (reflField.HasAttribute(REFL_ATTR_ENUM_MATCH_NAME)) {
			fieldName = reflField.GetAttribute(REFL_ATTR_ENUM_MATCH_NAME);
		}

		// Does the parameter map contain this field?
		if (!parameters.HasChild(fieldName)) {
			return;
		}

		const DeserializationParameterMap& fieldValue = parameters[fieldName];

		// Enum match?
		if (reflField.HasAttribute(REFL_ATTR_ENUM_MATCH_NAME)) {
			// Test if the enum value matches this field. If it does, continue on and deserialize.
			if (!ShouldDoEnumMatch(reflClass, reflField, obj, parameters)) {
				return;
			}
		}

		// Gets the starting address of this field.
		void* fieldStart = reflField.GetRawDataPtr(obj);

		// Class type?
		if (reflField.mTypeInfo.IsClass()) {
			ReflectionDeserializeClass(reflField.mTypeInfo.mClassType, *reflField.GetClass(), fieldStart, fieldValue);
		}
		// Enum type?
		else if (reflField.mTypeInfo.IsEnum()) {
			ReflectionDeserializeEnum(*reflField.GetEnum(), fieldStart, fieldValue);
		}
		// POD type?
		else if (reflField.mTypeInfo.IsPOD()) {
			ReflectionDeserializePOD(reflField.mTypeInfo, fieldStart, fieldValue, reflField.GetAttribute(REFL_ATTR_MULTIPLIER));
		}
		else {
			CORE_ASSERT(false, "Unrecognized reflection data type.");
		}
	}

	static void ReflectionDeserializeClass(const std::string& className, const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters)
	{
		// Special classes
		const bool handled = MaybeDeserializeReflectedClass(className, reflClass, obj, parameters);
		if (handled) {
			return;
		}

		// Deserialize each field.
		for (const auto& reflField : reflClass.mFields) {
			ReflectionDeserializeField(reflClass, reflField, obj, parameters);
		}
	}

	void ReflectionDeserialize(const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters)
	{
		ReflectionDeserializeClass(reflClass.mQualifiedName, reflClass, obj, parameters);
	}
}