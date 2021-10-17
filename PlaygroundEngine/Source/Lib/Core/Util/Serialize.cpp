#include "Serialize.h"

#include <algorithm>

#include "Color.h"
#include "Debug/Assert.h"
#include "File/File.h"
#include "Util/String.h"

#include "pugixml-1.9/pugixml.hpp"

#define RETURN_DEFAULT_IF_INVALID() if (*this == INVALID_DESERIALIZATION_PARAMETER) return def

namespace playground
{
	// Lol what is this
	DeserializationParameterMap INVALID_DESERIALIZATION_PARAMETER = { "~*INVALID*~" };

	const DeserializationParameterMap& DeserializationParameterMap::operator[](const std::string &childName)const
	{
		if (childrenMap.find(childName) == childrenMap.end()) {
			return INVALID_DESERIALIZATION_PARAMETER;
		}

		return childrenMap.at(childName);
	}

	const DeserializationParameterMap& DeserializationParameterMap::operator[](int index)const
	{
		CORE_ASSERT_RETURN_VALUE((size_t)index < childrenArray.size(), INVALID_DESERIALIZATION_PARAMETER, "Failed to find value at index %d", index);

		return childrenArray[index];
	}
	
	DeserializationParameterMap& DeserializationParameterMap::operator[](const std::string& childName)
	{
		if (childrenMap.find(childName) == childrenMap.end()) {
			return INVALID_DESERIALIZATION_PARAMETER;
		}

		return childrenMap.at(childName);
	}

	DeserializationParameterMap& DeserializationParameterMap::operator[](int index)
	{
		CORE_ASSERT_RETURN_VALUE((size_t)index < childrenArray.size(), INVALID_DESERIALIZATION_PARAMETER, "Failed to find value at index %d", index);

		return childrenArray[index];
	}

	void DeserializationParameterMap::Insert(const std::string &name, DeserializationParameterMap map)
	{
		childrenMap[name] = map;
	}

	void DeserializationParameterMap::Insert(int index, DeserializationParameterMap map)
	{
		childrenArray.insert(childrenArray.begin() + index, map);
	}

	bool DeserializationParameterMap::HasChild(const std::string &childName)const
	{
		return childrenMap.find(childName) != childrenMap.end();
	}

	std::string DeserializationParameterMap::AsString(const std::string &def)const
	{
		RETURN_DEFAULT_IF_INVALID();
		return value.c_str();
	}

	bool DeserializationParameterMap::AsBool(bool def)const
	{
		RETURN_DEFAULT_IF_INVALID();
		return value == "1" || value == "t" || value == "true" || value == "y" || value == "yes";
	}

	int DeserializationParameterMap::AsInt(int def)const
	{
		RETURN_DEFAULT_IF_INVALID();
		return std::stoi(value);
	}

	float DeserializationParameterMap::AsFloat(float def)const
	{
		RETURN_DEFAULT_IF_INVALID();
		return std::stof(value);
	}

	Vector2f DeserializationParameterMap::AsFloat2(Vector2f def)const
	{
		RETURN_DEFAULT_IF_INVALID();
		
		float x = GetChild("x").AsFloat(0.0f);
		float y = GetChild("y").AsFloat(0.0f);
		return Vector2f(x, y);
	}

	Vector3f DeserializationParameterMap::AsFloat3(Vector3f def)const
	{
		RETURN_DEFAULT_IF_INVALID();

		float x = GetChild("x").AsFloat(0.0f);
		float y = GetChild("y").AsFloat(0.0f);
		float z = GetChild("z").AsFloat(0.0f);
		return Vector3f(x, y, z);
	}

	Vector4f DeserializationParameterMap::AsFloat4(Vector4f def)const
	{
		RETURN_DEFAULT_IF_INVALID();

		float x = GetChild("x").AsFloat(0.0f);
		float y = GetChild("y").AsFloat(0.0f);
		float z = GetChild("z").AsFloat(0.0f);
		float w = GetChild("w").AsFloat(0.0f);
		return Vector4f(x, y, z, w);
	}

	Tuple2f DeserializationParameterMap::AsTuple2f(Tuple2f def)const
	{
		RETURN_DEFAULT_IF_INVALID();

		float x = GetChild("x").AsFloat(0.0f);
		float y = GetChild("y").AsFloat(0.0f);
		return Tuple2f(x, y);
	}

	Tuple3f DeserializationParameterMap::AsTuple3f(Tuple3f def)const
	{
		RETURN_DEFAULT_IF_INVALID();

		float x = GetChild("x").AsFloat(0.0f);
		float y = GetChild("y").AsFloat(0.0f);
		float z = GetChild("z").AsFloat(0.0f);
		return Tuple3f(x, y, z);
	}

	Tuple4f DeserializationParameterMap::AsTuple4f(Tuple4f def)const
	{
		RETURN_DEFAULT_IF_INVALID();

		float x = GetChild("x").AsFloat(0.0f);
		float y = GetChild("y").AsFloat(0.0f);
		float z = GetChild("z").AsFloat(0.0f);
		float w = GetChild("w").AsFloat(0.0f);
		return Tuple4f(x, y, z, w);
	}


	Vector4f DeserializationParameterMap::AsColor(Vector4f def)const
	{
		RETURN_DEFAULT_IF_INVALID();

		// String
		if (value.length() != 0) {
			ColorF color(value);
			return Vector4f(color.r, color.g, color.b, color.a);
		}

		float r = GetChild("r").AsFloat(0.0f);
		float g = GetChild("g").AsFloat(0.0f);
		float b = GetChild("b").AsFloat(0.0f);
		float a = GetChild("a").AsFloat(1.0f);
		return Vector4f(r, g, b, a);
	}

	Tuple4f DeserializationParameterMap::AsColorTuple(Tuple4f def)const
	{
		RETURN_DEFAULT_IF_INVALID();

		// String
		if (value.length() != 0) {
			ColorF color(value);
			return Tuple4f(color.r, color.g, color.b, color.a);
		}

		float r = GetChild("r").AsFloat(0.0f);
		float g = GetChild("g").AsFloat(0.0f);
		float b = GetChild("b").AsFloat(0.0f);
		float a = GetChild("a").AsFloat(1.0f);
		return Tuple4f(r, g, b, a);
	}

	float DeserializationParameterMap::AsRadian(float def)const
	{
		RETURN_DEFAULT_IF_INVALID();
		return ToRadians(AsFloat(def));
	}

	std::string DeserializationParameterMap::AsFilepath(const std::string &def)const
	{
		RETURN_DEFAULT_IF_INVALID();

		if (FileExists(value)) {
			std::string temp;
			GetFullFilepath(temp, value);
			return temp;
		} else {
			CORE_ASSERT(false, "Failed to find file %s", value.c_str());
		}

		return def;
	}

	void DeserializationParameterMap::AsHLSLType(void *out, const std::string &type)const
	{
#define COPY_INTO_BUFFER(s, T, f) if (type == s) { T v = f(); memcpy(out, &v, sizeof(T)); found = true; }
		bool found = false;
		COPY_INTO_BUFFER("float", float, AsFloat);
		COPY_INTO_BUFFER("float2", Vector2f, AsFloat2);
		COPY_INTO_BUFFER("float3", Vector3f, AsFloat3);
		COPY_INTO_BUFFER("float4", Vector4f, AsFloat4);
		COPY_INTO_BUFFER("color", Vector4f, AsColor);
		CORE_ASSERT(found, "Type %s is not recognized.", type.c_str());
#undef COPY_INTO_BUFFER
	}


	bool DeserializationParameterMap::HasAttribute(const std::string &name)const
	{
		return meta.find(name) != meta.end();
	}

	std::string DeserializationParameterMap::GetAttribute(const std::string& name)const
	{
		if (!HasAttribute(name)) {
			return "";
		}

		return meta.at(name);
	}




	static DeserializationParameterMap ParseParameterXML(pugi::xml_node &parent)
	{
		DeserializationParameterMap parentParam;
		parentParam.value = parent.text().as_string();
		StrTrim(parentParam.value);

		// Is array?
		bool isArray = false;
		const char *lastName = nullptr;
		for (auto &child : parent.children()) {
			if (lastName) {
				if (strcmp(lastName, child.name()) == 0) {
					isArray = true;
					break;
				}
			}
			lastName = child.name();
		}

		// Gather attributes (meta)
		for (auto &attr : parent.attributes()) {
			parentParam.meta[attr.name()] = attr.value();
		}

		// Gather properties
		for (auto &child : parent.children()) {
			if (child.type() == pugi::xml_node_type::node_element) {
				if (isArray) {
					parentParam.childrenArray.push_back(ParseParameterXML(child));
				} else {
					parentParam.childrenMap[child.name()] = ParseParameterXML(child);
				}
			}
		}

		if (parentParam.childrenMap.size() == 1) {
			parentParam.childrenArray.push_back(parentParam.childrenMap.begin()->second);
		}

		return parentParam;
	}

	DeserializationParameterMap ParseNode(pugi::xml_node &parent)
	{
		DeserializationParameterMap params;
		for (auto &param : parent.children()) {
			params.Insert(param.name(), ParseParameterXML(param));
		}

		return params;
	}

	DeserializationParameterMap ParseFile(const std::string &filename)
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(filename.c_str());
		CORE_ASSERT_RETURN_VALUE(result, INVALID_DESERIALIZATION_PARAMETER, "Failed to load XML file '%s'", filename.c_str());

		pugi::xml_node root = doc.child("root");
		return ParseNode(root);
	}
}