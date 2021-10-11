#pragma once

#include "ReflectionElement.h"

namespace refl
{
	// Represents an individual enum value in a reflected enum type.
	class EnumValue : public Element
	{
	public:
		using Element::DeepEquals;

		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const EnumValue& rhs)const;

		// Read or write this element.
		virtual void Serialize(class FileIO& io) override;

#if defined(REFL_DEBUG)
		// Creates a string representation of this enum value.
		virtual std::string ToString(int indent = 0)const override;
#endif

	public:
		// The integer value of this enum value.
		int mValue;
	};

	// Represents a reflected enum type.
	class Enum : public Element
	{
	public:
		using Element::DeepEquals;

		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Enum& rhs)const;

		// Read or write this element.
		virtual void Serialize(class FileIO& io) override;

		// Returns the name of an enum value.
		std::string GetValueString(int enumValue, bool qualified = false)const;

#if defined(REFL_DEBUG)
		// Creates a string representation of this enum.
		virtual std::string ToString(int indent = 0)const override;
#endif

	public:
		// Table mapping integer values to enum values.
		std::map<int, EnumValue> mValueTable;
	};
}