#pragma once

#include "ReflectionElement.h"

namespace refl
{
	// Represents a reflected field in a reflected class.
	class Field : public Element
	{
	public:
		using Element::DeepEquals;

		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Field& rhs)const;

		// Read or write this element.
		virtual void Serialize(class FileIO& io) override;

		// Returns a pointer to the start of this field in a buffer of data.
		void* GetRawDataPtr(void* obj)const;

		// Returns a strongly-typed pointer to the start of this field in a buffer of data.
		template <typename T>
		T* GetDataPtr(void* obj)const;

		// Returns a string pointer to the start of this field in a buffer of data.
		// NB: This is equivalent to `char* str = GetDataPtr<char>(obj);`
		char* GetString(void* obj)const;

		template <typename T>
		T* GetArrayElement(void* obj, int index)const;

#if defined(REFL_DEBUG)
		// Creates a string representation of this field.
		virtual std::string ToString(int indent = 0)const override;
#endif

		// Is this field a class type?
		bool IsClassType()const { return mTypeInfo.mClassType != ""; }
		// Is this field an enum type?
		bool IsEnumType()const { return mTypeInfo.mEnumType != ""; }
		// Is this field a primitive type?
		bool IsPrimitive()const { return !IsClassType() && !IsEnumType(); };

		// Returns this field's class type, if it is a class.
		const class Class* GetClass()const;
		// Returns this field's enum type, if it is a class.
		const class Enum* GetEnum()const;

		// Is this field const?
		bool IsConst()const { return mTypeInfo.mIsConst; }
		// Is this field a pointer?
		bool IsPointer()const { return mTypeInfo.mIsPointer; }

		// Is this field a fixed size array?
		bool IsFixedArray()const { return mTypeInfo.IsFixedArray(); }
		// Is this field a dynamic array? (i.e. std::vector)
		bool IsDynamicArray()const { return mTypeInfo.IsDynamicArray(); }

		// Is this field a fixed size string?
		bool IsFixedString()const { return mTypeInfo.IsFixedString(); }
		// Is this field a dynamic string? (i.e. std::string)
		bool IsDynamicString()const { return mTypeInfo.IsDynamicString(); }

		// Is this field templated?
		bool IsTemplated()const { return mTypeInfo.IsTemplated(); }

	public:
		// Field type.
		TypeInfo mTypeInfo;

		// Offset into the containing class.
		size_t mOffset;
	};
}