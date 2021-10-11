#pragma once

#include <string>
#include <vector>

#include "./Private/FunctionRegistration.h"

namespace refl
{
	// Field data for things not known until runtime.
	struct FieldRegistration
	{
		FieldRegistration(const char* name, uint32_t offset, uint32_t size) :
			mOffset(offset),
			mSize(size),
			mFieldName(name)
		{}

		// Field offset into owning struct (in bytes)
		uint32_t mOffset;

		// Field size (in bytes)
		uint32_t mSize;

		// Field name
		const char* mFieldName;
	};

	// Class data for things not known until runtime.
	struct ClassRegistration
	{
		ClassRegistration(const char* name, uint32_t size) :
			mClassName(name),
			mSize(size)
		{
		}

		void RegisterField(const FieldRegistration& field)
		{
			mFields.push_back(field);
		}

		void RegisterFunction(const FunctionRegistration& function)
		{
			mFunctions.push_back(function);
		}

		// Class name
		const char* mClassName;

		// Class size (in bytes)
		uint32_t mSize;

		// Class members
		std::vector<FieldRegistration> mFields;
		std::vector<FunctionRegistration> mFunctions;
	};
}