#pragma once

#include <string>
#include <map>
#include <vector>

#include "Core/Math/Vector.h"
#include "Core/Util/Util.h"

namespace pugi {
	class xml_node;
}

namespace playground
{
	struct DeserializationParameterMap
	{
		std::string value;
		std::map<std::string, std::string> meta;

		std::map<std::string, DeserializationParameterMap> childrenMap;
		std::vector<DeserializationParameterMap> childrenArray;

		DeserializationParameterMap& operator[](const std::string &childName);
		DeserializationParameterMap& operator[](int index);
		
		void Insert(const std::string &name, DeserializationParameterMap value);
		void Insert(int index, DeserializationParameterMap value);

		inline bool operator==(const DeserializationParameterMap &rhs) { return value == rhs.value; }

		bool HasChild(const std::string &childName);

		std::string AsString(const std::string &def = "");
		bool AsBool(bool def = false);
		int AsInt(int def = 0);
		
		float AsFloat(float def = 0.0f);
		Vector2f AsFloat2(Vector2f def = Vector2f(0.0f, 0.0f));
		Vector3f AsFloat3(Vector3f def = Vector3f(0.0f, 0.0f, 0.0f));
		Vector4f AsFloat4(Vector4f def = Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

		Tuple2f AsTuple2f(Tuple2f = Tuple2f(0.0f, 0.0f));
		Tuple3f AsTuple3f(Tuple3f = Tuple3f(0.0f, 0.0f, 0.0f));
		Tuple4f AsTuple4f(Tuple4f = Tuple4f(0.0f, 0.0f, 0.0f, 0.0f));

		Vector4f AsColor(Vector4f def = Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
		Tuple4f AsColorTuple(Tuple4f def = Tuple4f(1.0f, 0.0f, 0.0f, 1.0f));

		float AsRadian(float def = 0.0f);

		std::string AsFilepath(const std::string &def = "");

		void AsHLSLType(void *out, const std::string &type);

		bool HasAttribute(const std::string &name);

	private:
		inline DeserializationParameterMap& GetChild(const std::string &childName) { return this->operator[](childName); }
	};

	extern DeserializationParameterMap INVALID_DESERIALIZATION_PARAMETER;
	
	DeserializationParameterMap ParseNode(pugi::xml_node &parent);
	DeserializationParameterMap ParseFile(const std::string &filename);
}