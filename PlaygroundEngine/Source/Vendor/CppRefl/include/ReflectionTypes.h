#pragma once

#include <string>
#include <vector>

namespace refl
{
	#undef VOID

	// All supported data types.
	enum class DataType : uint8_t
	{
		INVALID = 0,

		BOOL = 1,

		UINT8,
		INT8,
		UINT16,
		INT16,
		UINT32,
		INT32,
		UINT64,
		INT64,

		FLOAT,
		DOUBLE,
		LONG_DOUBLE,

		UNION,

		CLASS, // struct, class, record, whatever

		VOID, // for function return values
		
		// Special markers
		POD_MIN = BOOL,
		POD_MAX = LONG_DOUBLE,
		INT_TYPE_MIN = UINT8,
		INT_TYPE_MAX = INT64,
	};

	// Contains info about a type (e.g. data type, constness, etc.)
	class TypeInfo
	{
	public:
		// Data type.
		DataType mDataType = DataType::INVALID;
		
		// Size of the data type.
		size_t mSize = -1;

		// Number of elements in this array.
		size_t mArraySize = -1;

		// Underlying class type, if this type is a class.
		std::string mClassType;
		// Underlying enum type, if this type is an enum.
		std::string mEnumType;

		// Is this field const?
		bool mIsConst = false;
		// Is this field a pointer?
		bool mIsPointer = false;

		// Is this type a dynamic array/string?
		bool mIsDynamic = false;

		// All templated types
		std::vector<TypeInfo> mTemplateTypes;

	public:
		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		bool DeepEquals(const TypeInfo& rhs)const;

		// Read or write this element.
		void Serialize(class FileIO& io);

		// Is this type a fixed sized array?
		bool IsFixedArray()const { return mArraySize != -1; }
		// Is this type a dynamic array?
		bool IsDynamicArray()const { return mClassType == "std::vector"; }

		// Is this type a fixed sized string?
		bool IsFixedString()const { return IsFixedArray() && mDataType == DataType::INT8; }
		// Is this type a dynamic string?
		bool IsDynamicString()const { return mClassType == "std::string"; }

		// Is this type a templated type?
		bool IsTemplated()const { return !mTemplateTypes.empty(); }

		// Is this type an integer type?
		bool IsInteger()const;

		// Is this type an unsigned type?
		bool IsUnsigned()const;

		// Is this type a real number type (i.e. float or double)?
		bool IsReal()const;

		// Is this type a class?
		bool IsClass()const;

		// Is this type an enum?
		bool IsEnum()const;

		// Is this type a piece of data type?
		bool IsPOD()const;
	};
}