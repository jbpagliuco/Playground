#pragma once

#include "ReflectionElement.h"
#include "ReflectionField.h"
#include "ReflectionFunction.h"

namespace refl
{
	typedef void (*ClassConstructor)(void* obj);
	typedef void (*ClassDestructor)(void* obj);

	// Represents a reflected class or struct.
	class Class : public Element
	{
	public:
		using Element::DeepEquals;

		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Class& rhs)const;

		// Get a field by name.
		const Field* GetField(const std::string& fieldName)const;
		Field* GetField(const std::string& fieldName);

		// Get a function by name.
		const Function* GetFunction(const std::string& functionName)const;
		Function* GetFunction(const std::string& functionName);

		// Construct an object of this type over the given memory.
		void* Construct(void* obj)const;

		template <typename T>
		T* Construct(void* obj)const
		{
			return static_cast<T*>(Construct(obj));
		}

		// Destruct an object of this type over the given memory.
		void Destruct(void* obj)const;

		// Read or write this element.
		virtual void Serialize(class FileIO& io) override;

#if defined(REFL_DEBUG)
		// Creates a string representation of this class.
		virtual std::string ToString(int indent = 0)const override;
#endif

	public:
		// List of all fields in this class.
		std::vector<Field> mFields;
		// List of all functions in this class.
		std::vector<Function> mFunctions;

		// This size of this class.
		size_t mSize;

		// Constructor/Destructor
	private:
		ClassConstructor mConstructor;
		ClassDestructor mDestructor;

		friend class Registry;
	};
}