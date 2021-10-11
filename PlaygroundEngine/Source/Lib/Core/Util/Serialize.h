#pragma once

#include <string>
#include <map>
#include <vector>

#include "ReflectionClass.h"

#include "Core/Math/Vector.h"
#include "Core/Util/Util.h"

namespace pugi {
	class xml_node;
}

namespace playground
{
	class DeserializationParameterMap
	{
	public:
		// Retrieve data by name.
		const DeserializationParameterMap& operator[](const std::string &childName)const;
		// Retrieve data by array index.
		const DeserializationParameterMap& operator[](int index)const;

		// Retrieve data by name.
		DeserializationParameterMap& operator[](const std::string& childName);
		// Retrieve data by array index.
		DeserializationParameterMap& operator[](int index);
		
		// Insert an element by name.
		void Insert(const std::string &name, DeserializationParameterMap value);
		// Insert an element by array index.
		void Insert(int index, DeserializationParameterMap value);

		// Equality operator.
		inline bool operator==(const DeserializationParameterMap &rhs)const { return value == rhs.value; }

		// Does this map have a child with the given name?
		bool HasChild(const std::string &childName)const;

		// Return this value as a string.
		std::string AsString(const std::string &def = "")const;
		// Return this value as a boolean.
		bool AsBool(bool def = false)const;
		// Return this value as an integer.
		int AsInt(int def = 0)const;

		// Return this value as a float.
		float AsFloat(float def = 0.0f)const;
		// Return this value as a float2.
		Vector2f AsFloat2(Vector2f def = Vector2f(0.0f, 0.0f))const;
		// Return this value as a float3.
		Vector3f AsFloat3(Vector3f def = Vector3f(0.0f, 0.0f, 0.0f))const;
		// Return this value as a float4.
		Vector4f AsFloat4(Vector4f def = Vector4f(0.0f, 0.0f, 0.0f, 0.0f))const;

		// Return this value as a tuple2.
		Tuple2f AsTuple2f(Tuple2f = Tuple2f(0.0f, 0.0f))const;
		// Return this value as a tuple3.
		Tuple3f AsTuple3f(Tuple3f = Tuple3f(0.0f, 0.0f, 0.0f))const;
		// Return this value as a tuple4.
		Tuple4f AsTuple4f(Tuple4f = Tuple4f(0.0f, 0.0f, 0.0f, 0.0f))const;

		// Return this value as a color.
		Vector4f AsColor(Vector4f def = Vector4f(1.0f, 0.0f, 0.0f, 1.0f))const;
		// Return this value as a color tuple.
		Tuple4f AsColorTuple(Tuple4f def = Tuple4f(1.0f, 0.0f, 0.0f, 1.0f))const;

		// Return this value as a float in radians.
		float AsRadian(float def = 0.0f)const;

		// Return this value as a filepath.
		std::string AsFilepath(const std::string &def = "")const;

		// Return this value as an HLSL type.
		void AsHLSLType(void *out, const std::string &type)const;

		// Does this parameter map have the specified attribute?
		bool HasAttribute(const std::string& name)const;

		// Get the specified attribute value.
		std::string GetAttribute(const std::string& name)const;

	public:
		std::string value;
		std::map<std::string, std::string> meta;

		std::map<std::string, DeserializationParameterMap> childrenMap;
		std::vector<DeserializationParameterMap> childrenArray;

	private:
		inline const DeserializationParameterMap& GetChild(const std::string &childName)const { return this->operator[](childName); }
	};

	extern DeserializationParameterMap INVALID_DESERIALIZATION_PARAMETER;
	
	DeserializationParameterMap ParseNode(pugi::xml_node &parent);
	DeserializationParameterMap ParseFile(const std::string &filename);

	// Deserialize a parameter map into an object, according to its reflected data.
	void ReflectionDeserialize(const refl::Class& reflClass, void* obj, const DeserializationParameterMap& parameters);
}