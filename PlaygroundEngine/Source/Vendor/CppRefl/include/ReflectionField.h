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
		virtual void Serialize(class FileIO& io, const class Registry& registry) override;


		// Returns a pointer to the start of this field in a buffer of data.
		void* GetDataPtr(void* obj)const;

		// Returns a strongly-typed pointer to the start of this field in a buffer of data.
		template <typename T>
		T* GetDataPtr(void* obj)const;


		// Returns a string pointer to the start of this field in a buffer of data.
		// NB: This is equivalent to `char* str = GetDataPtr<char>(obj);`
		char* GetString(void* obj)const;


		// Gets the dynamic array accessors for this field type.
		const struct DynamicArrayAccessors* GetDynamicArrayAccessors()const;

		// Returns a pointer to the location of the index-th element of the array represented by obj.
		void* GetArrayElement(void* obj, int index)const;

		// Returns a pointer to the location of the index-th element of the array represented by obj.
		template <typename T>
		T* GetArrayElement(void* obj, int index)const;

		// Returns the size of the array represented by obj.
		size_t GetArraySize(void* obj)const;



		// Returns this field's class type, if it is a class.
		const class Class* GetClass()const;
		// Returns this field's enum type, if it is a class.
		const class Enum* GetEnum()const;

#if defined(REFL_DEBUG)
		// Creates a string representation of this field.
		virtual std::string ToString(int indent = 0)const override;
#endif

	public:
		// Field type.
		TypeInstance mType;

		// Offset into the containing class.
		size_t mOffset;
	};
}

#include "ReflectionField.inl"