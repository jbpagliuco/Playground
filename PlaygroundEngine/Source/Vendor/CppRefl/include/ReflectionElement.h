#pragma once

#include <map>
#include <string>

#include "ReflectionTypes.h"

namespace refl
{
	////////////////////////////////////////////
	// Classes
	////////////////////////////////////////////

	// Base class for all reflected data.
	class Element
	{
	public:
		Element();

		// Shallow check for equality/inequality.
		virtual bool operator==(const Element& rhs)const;
		virtual bool operator!=(const Element& rhs)const;

		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Element& rhs)const;

		bool HasAttribute(const std::string& attributeName)const;
		std::string GetAttribute(const std::string& attributeName)const;

		// Read or write this element.
		virtual void Serialize(class FileIO& io);

#if defined(REFL_DEBUG)
		// Creates a string representation of this element.
		virtual std::string ToString(int indent = 0)const;

	protected:
		// A human-readable string containing all of our attributes.
		std::string GetAttrString()const;
#endif

	public:
		// Reflected name of this element.
		std::string mName;

		// Fully qualified name of this element (includes namespaces and classes).
		std::string mQualifiedName;

		// Reflected attributes of this element.
		std::map<std::string, std::string> mAttributes;
	};
}