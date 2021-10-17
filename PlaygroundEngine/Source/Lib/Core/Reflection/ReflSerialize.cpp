#include "ReflSerialize.h"

#include "Debug/Assert.h"
#include "Reflection/ReflMarkup.h"
#include "Streaming/Stream.h"
#include "Util/Color.h"
#include "Util/String.h"

namespace playground
{
	struct FieldDeserializationParams
	{
		// The name of the class that contains this field.
		std::string className;

		// The class that contains this field.
		const refl::Class* reflClass = nullptr;

		// The pointer to the start of the class.
		void* classObj = nullptr;

		// Deserialization map to the class.
		DeserializationParameterMap classParameters;

		// The field to deserialize.
		const refl::Field* reflField = nullptr;

		// The pointer to the start of the field.
		void* fieldObj = nullptr;

		// Deserialization map to the field.
		DeserializationParameterMap fieldParameters;
	};

	static void ReflectionDeserializeFieldRaw(const refl::Field& reflField, void* fieldObj, const DeserializationParameterMap& fieldValue, const refl::TypeInstance& type);
	static void ReflectionDeserializeArray(const FieldDeserializationParams& parameters);
	static void ReflectionDeserializeField(const FieldDeserializationParams& parameters, const refl::TypeInstance& type);

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



	static void ReflectionDeserializeFieldRaw(const refl::Field& reflField, void* fieldObj, const DeserializationParameterMap& fieldValue, const refl::TypeInstance& type)
	{
		// Class type?
		if (type.IsClass()) {
			ReflectionDeserializeClass(type.GetName(), *type.GetClass(), fieldObj, fieldValue, &reflField);
		}
		// Enum type?
		else if (reflField.mType.IsEnum()) {
			ReflectionDeserializeEnum(*type.GetEnum(), fieldObj, fieldValue);
		}
		// POD type?
		else if (reflField.mType.IsPOD()) {
			ReflectionDeserializePOD(*type.mType, fieldObj, fieldValue, reflField.GetAttribute(REFL_ATTR_MULTIPLIER));
		}
		else {
			CORE_ASSERT(false, "Unrecognized reflection data type.");
		}
	}

	static void ReflectionDeserializeArray(const FieldDeserializationParams& fieldParameters)
	{
		const auto& elementType = fieldParameters.reflField->mType.GetDynamicArrayElementType();

		for (int i = 0; i < fieldParameters.fieldParameters.childrenArray.size(); ++i) {
			// Append a value to the array
			fieldParameters.reflField->GetDynamicArrayAccessors()->mAppendFunction(fieldParameters.fieldObj);

			void* fieldStart = fieldParameters.reflField->GetArrayElement(fieldParameters.fieldObj, i);

			ReflectionDeserializeFieldRaw(*fieldParameters.reflField, fieldStart, fieldParameters.fieldParameters.childrenArray[i], elementType);
		}
	}

	static void ReflectionDeserializeField(const FieldDeserializationParams& parameters, const refl::TypeInstance& type)
	{
		FieldDeserializationParams fieldParameters = parameters;

		// Get reflected name
		const std::string fieldName = GetReflName(*parameters.reflField);

		// Pointer to the start of this field
		fieldParameters.fieldObj = parameters.reflField->GetDataPtr(parameters.classObj);

		// Bool enable?
		if (fieldParameters.reflField->HasAttribute(REFL_ATTR_BOOL_ENABLE)) {
			const std::string enableVar = fieldParameters.reflField->GetAttribute(REFL_ATTR_BOOL_ENABLE);
			const bool enable = fieldParameters.classParameters.HasChild(GetReflName(*fieldParameters.reflClass->GetField(enableVar)));
			WriteReflectedValue<bool>(fieldParameters.fieldObj, enable);
			return;
		}

		// Does the parameter map contain this field?
		if (!parameters.classParameters.HasChild(fieldName)) {
			return;
		}

		// Find field parameters
		fieldParameters.fieldParameters = parameters.classParameters[fieldName];

		if (fieldParameters.reflField->mType.IsDynamicArray()) {
			ReflectionDeserializeArray(fieldParameters);
		}
		else {
			// Enum match?
			if (fieldParameters.reflField->HasAttribute(REFL_ATTR_ENUM_MATCH_NAME)) {
				// Test if the enum value matches this field. If it does, continue on and deserialize.
				if (!ShouldDoEnumMatch(*fieldParameters.reflClass, *fieldParameters.reflField, fieldParameters.classObj, fieldParameters.classParameters)) {
					return;
				}
			}

			ReflectionDeserializeFieldRaw(*fieldParameters.reflField, fieldParameters.fieldObj, fieldParameters.fieldParameters, fieldParameters.reflField->mType);
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
		FieldDeserializationParams fieldParameters;
		fieldParameters.className = className;
		fieldParameters.reflClass = &reflClass;
		fieldParameters.classObj = obj;
		fieldParameters.classParameters = parameters;
		for (const auto& reflField : reflClass.mFields) {
			fieldParameters.reflField = &reflField;
			ReflectionDeserializeField(fieldParameters, reflField.mType);
		}
	}

	void ReflectionDeserialize(const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters)
	{
		ReflectionDeserializeClass(reflClass.mQualifiedName, reflClass, obj, parameters);
	}
}