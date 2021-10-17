#pragma once

#include <string>
#include <vector>

namespace refl
{
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

		ENUM,

		UNION,
		
		// struct, class, record, whatever
		CLASS,

		
		// void return type (can't just be VOID because windows has a macro for that :))))))) )
		VOID_TYPE,
		
		// Special markers
		POD_MIN = BOOL,
		POD_MAX = LONG_DOUBLE,
		INT_TYPE_MIN = UINT8,
		INT_TYPE_MAX = INT64,
	};

	// An instance of a type. Contains info about modifiers (pointer, constness, etc.).
	class TypeInstance
	{
	public:
		// Returns the data type of this type.
		DataType GetDataType()const;

		// Returns the size of this type.
		size_t GetSize()const;

		// Returns the name of this type.
		std::string GetName()const;


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

		// Is this type a dynamic array? (i.e. std::vector)
		bool IsDynamicArray()const;

		// Is this type a dynamic string?
		bool IsDynamicString()const;

		// Is this type a templated type?
		bool IsTemplated()const;


		// Is this type a fixed sized array?
		bool IsFixedArray()const;

		// Is this type a fixed sized string?
		bool IsFixedString()const;


		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		bool DeepEquals(const TypeInstance& rhs)const;

		// Read or write this element.
		void Serialize(class FileIO& io, const class Registry& registry);

	public:
		// Info about the base type.
		const class Type* mType = nullptr;

		// Is this field const?
		bool mIsConst = false;

		// Is this field a pointer?
		bool mIsPointer = false;

		// Number of elements in this array.
		size_t mArraySize = -1;
	};

	// Contains info about a type (e.g. int, bool, MyClass, etc.)
	class Type
	{
	public:
		Type();
		Type(DataType dataType);

		// Test equality with another type
		bool operator==(const Type& type)const;

		// Test equality with another type
		bool operator<(const Type& type)const;


		// Returns the data type of this type.
		DataType GetDataType()const;

		// Returns the size of this type.
		size_t GetSize()const;

		// Returns the name of this type.
		std::string GetName()const;


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

		// Is this type a dynamic array? (i.e. std::vector)
		bool IsDynamicArray()const;

		// Is this type a dynamic string?
		bool IsDynamicString()const;

		// Is this type a templated type?
		bool IsTemplated()const;


		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		bool DeepEquals(const Type& rhs)const;

		// Read or write this element.
		void Serialize(class FileIO& io, const class Registry& registry);


	public:
		// Data type.
		DataType mDataType = DataType::INVALID;

		// Size of this type.
		size_t mSize = -1;

		// Type name.
		std::string mName;

		// Template parameter types, if this is a templated class.
		std::vector<TypeInstance> mTemplateParameters;
	};
}