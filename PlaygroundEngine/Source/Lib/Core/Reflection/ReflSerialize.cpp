#include "ReflSerialize.h"

#include "Debug/Assert.h"
#include "Reflection/ReflMarkup.h"
#include "Streaming/Stream.h"
#include "Util/Color.h"
#include "Util/String.h"

namespace playground
{
	static void ReflectionDeserializeFieldRaw(const refl::Class& reflClass, const refl::Field& reflField, const refl::TypeInstance& type, void* obj, const DeserializationParameterMap& fieldValue);
	static void ReflectionDeserializeField(const refl::Class& reflClass, const refl::Field& reflField, void* obj, const DeserializationParameterMap& parameters);
	static void ReflectionDeserializeClass(const std::string& className, const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters, const refl::Field* reflField = nullptr);

	template <typename ReflectedType, typename ValueType>
	static void WriteReflectedValue(void* obj, const ValueType& value)
	{
		*(static_cast<ReflectedType*>(obj)) = static_cast<ReflectedType>(value);
	}

	static std::string GetReflName(const refl::Element& reflElement)
	{
		if (reflElement.HasAttribute(REFL_ATTR_ENUM_MATCH_NAME)) {
			return reflElement.GetAttribute(REFL_ATTR_ENUM_MATCH_NAME);
		}

		return reflElement.HasAttribute(REFL_ATTR_NAME) ? reflElement.GetAttribute(REFL_ATTR_NAME) : reflElement.mName;
	}


	static void ReflectionDeserializePOD(const refl::Type& typeInfo, void* obj, const DeserializationParameterMap& value, const std::string& multiplierVal)
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
		for (const auto& it : reflEnum.mValueTable) {
			const int8_t value = it.first;
			const refl::EnumValue& enumValue = it.second;

			if (GetReflName(enumValue) == parameters.value) {
				WriteReflectedValue<int8_t>(obj, value);
				return;
			}
		}

		CORE_ASSERT(false, "Failed to find value '%s' within enum '%s'.", parameters.value, reflEnum.mQualifiedName.c_str());
	}

	static bool MaybeDeserializeReflectedClass(const std::string className, const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters, const refl::Field* reflField = nullptr)
	{
		// std library classes
		if (className == "std::string") {
			const std::string value = (reflField != nullptr && reflField->HasAttribute(REFL_ATTR_FILEPATH)) ? parameters.AsFilepath() : parameters.value;
			WriteReflectedValue<std::string>(obj, value);
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



	static void ReflectionDeserializeFieldRaw(const refl::Class& reflClass, const refl::Field& reflField, const refl::TypeInstance& type, void* obj, const DeserializationParameterMap& fieldValue)
	{
		// Class type?
		if (type.IsClass()) {
			ReflectionDeserializeClass(type.GetName(), *type.GetClass(), obj, fieldValue, &reflField);
		}
		// Enum type?
		else if (reflField.mType.IsEnum()) {
			ReflectionDeserializeEnum(*type.GetEnum(), obj, fieldValue);
		}
		// POD type?
		else if (reflField.mType.IsPOD()) {
			ReflectionDeserializePOD(*type.mType, obj, fieldValue, reflField.GetAttribute(REFL_ATTR_MULTIPLIER));
		}
		else {
			CORE_ASSERT(false, "Unrecognized reflection data type.");
		}
	}

	static void ReflectionDeserializeArray(const refl::Class& reflClass, const refl::Field& reflField, void* obj, const DeserializationParameterMap& parameters)
	{
		for (int i = 0; i < parameters.childrenArray.size(); ++i) {
			reflField.GetDynamicArrayAccessors()->mAppendFunction(obj);
			ReflectionDeserializeFieldRaw(reflClass, reflField, reflField.mType.GetDynamicArrayElementType(), reflField.GetArrayElement(obj, i), parameters.childrenArray[i]);
		}
	}

	static void ReflectionDeserializeField(const refl::Class& reflClass, const refl::Field& reflField, void* obj, const DeserializationParameterMap& parameters)
	{
		void* fieldPtr = reflField.GetDataPtr(obj);

		if (reflField.mType.IsDynamicArray()) {
			ReflectionDeserializeArray(reflClass, reflField, fieldPtr, parameters);
		}
		else {
			// Get reflected name
			const std::string fieldName = GetReflName(reflField);

			// Bool enable?
			if (reflField.HasAttribute(REFL_ATTR_BOOL_ENABLE)) {
				const std::string enableVar = reflField.GetAttribute(REFL_ATTR_BOOL_ENABLE);
				const bool enable = parameters.HasChild(GetReflName(*reflClass.GetField(enableVar)));
				WriteReflectedValue<bool>(fieldPtr, enable);
				return;
			}

			// Does the parameter map contain this field?
			if (!parameters.HasChild(fieldName)) {
				return;
			}

			const DeserializationParameterMap& fieldValue = parameters[fieldName];

			// Enum match?
			if (reflField.HasAttribute(REFL_ATTR_ENUM_MATCH_NAME)) {
				// Test if the enum value matches this field. If it does, continue on and deserialize.
				if (!ShouldDoEnumMatch(reflClass, reflField, fieldPtr, parameters)) {
					return;
				}
			}

			ReflectionDeserializeFieldRaw(reflClass, reflField, reflField.mType, fieldPtr, fieldValue);
		}
	}

	static void ReflectionDeserializeClass(const std::string& className, const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters, const refl::Field* reflField)
	{
		// Special classes
		const bool handled = MaybeDeserializeReflectedClass(className, reflClass, obj, parameters, reflField);
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