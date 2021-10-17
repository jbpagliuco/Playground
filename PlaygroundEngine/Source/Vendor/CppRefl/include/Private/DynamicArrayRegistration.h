#pragma once

#include <string>
#include <vector>

#include "./Private/FunctionRegistration.h"

namespace refl
{
	typedef void* (*DynamicArrayAccess)(void* obj, int index);
	typedef void* (*DynamicArrayAppend)(void* obj);
	typedef size_t (*DynamicArrayGetSize)(void* obj);


	template <typename T>
	void* DynamicArrayAccessor_Access(void* obj, int index);
	template <typename T>
	void* DynamicArrayAccessor_Append(void* obj);
	template <typename T>
	size_t DynamicArrayAccessor_GetSize(void* obj);


	// Functions that allow access to templated dynamic arrays.
	struct DynamicArrayAccessors
	{
	public:
		template <typename T>
		static DynamicArrayAccessors Create()
		{
			DynamicArrayAccessors accessors;

			accessors.mAccessFunction = &DynamicArrayAccessor_Access<T>;
			accessors.mAppendFunction = &DynamicArrayAccessor_Append<T>;
			accessors.mGetSizeFunction = &DynamicArrayAccessor_GetSize<T>;

			return accessors;
		}

		// Function for accessing an element at an index.
		DynamicArrayAccess mAccessFunction;

		// Function for appending an element to the end of an array.
		DynamicArrayAppend mAppendFunction;

		// Function for accessing the size of an array.
		DynamicArrayGetSize mGetSizeFunction;
	};

	// Registration for types to be used in dynamic arrays (i.e. std::vector)
	struct DynamicArrayRegistration
	{
	public:
		DynamicArrayRegistration(const char* typeName, DynamicArrayAccessors accessors) : 
			mTypeName(typeName),
			mAccessors(accessors)
		{}

		std::string mTypeName;
		DynamicArrayAccessors mAccessors;
	};




	template <typename T>
	void* DynamicArrayAccessor_Access(void* obj, int index)
	{
		std::vector<T>* vector = static_cast<std::vector<T>*>(obj);
		if (index < 0 || index >= vector->size()) {
			REFL_INTERNAL_RAISE_ERROR("Tried to access invalid element at index %d", index);
			return nullptr;
		}

		return &vector->at(index);
	}

	template <typename T>
	void* DynamicArrayAccessor_Append(void* obj)
	{
		std::vector<T>* vector = static_cast<std::vector<T>*>(obj);
		return &vector->emplace_back();
	}

	template <typename T>
	size_t DynamicArrayAccessor_GetSize(void* obj)
	{
		const std::vector<T>* vector = static_cast<std::vector<T>*>(obj);
		return vector->size();
	}
}