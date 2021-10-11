#pragma once

#include <map>
#include <string>

#include "ReflectionClass.h"
#include "ReflectionEnum.h"
#include "ReflectionFunction.h"
#include "./Private/ClassRegistration.h"
#include "./Private/Util.h"

namespace refl
{
	// Contains all reflected data for a translation unit.
	class Registry
	{
	public:
		Registry();

		// Finalizes the data in this registry (must be called after all reflected data is registered).
		bool Finalize();

		// Retrieves a reflected class representation from this registry, or nullptr if it does not exist.
		const Class* GetClass(const std::string& className)const;
		// Retrieves a reflected enum representation from this registry, or nullptr if it does not exist.
		const Enum* GetEnum(const std::string& enumName)const;
		// Retrieves a reflected global function representation from this registry, or nullptr if it does not exist.
		const Function* GetFunction(const std::string& functionName)const;

		// Does this registry contain the specified Class?
		bool HasClass(const std::string& className)const;
		// Does this registry contain the specified Enum?
		bool HasEnum(const std::string& enumName)const;
		// Does this registry contain the specified Function?
		bool HasFunction(const std::string& functionName)const;

		// Adds a reflected class to this registry.
		Class& CreateClass(const std::string& name);
		// Adds a reflected enum to this registry.
		Enum& CreateEnum(const std::string& name);
		// Adds a reflected function to this registry.
		Function& CreateFunction(const std::string& name);

		// Finalizes a reflected class in this registry.
		bool FinalizeClass(const ClassRegistration& classRegistration);
		// Finalizes a reflected function in this registry.
		bool FinalizeFunction(const FunctionRegistration& functionRegistration);

		// Serialize this registry to a file.
		bool Export(const std::string& filename);
		// Import registry data from a file.
		bool Import(const std::string& filename);

		// Checks for equality among every aspect of this registry.
		// NB: This is really only necessary for testing purposes.
		bool DeepEquals(const Registry& rhs)const;

	private:
		// Retrieves a reflected class representation from this registry, or nullptr if it does not exist.
		Class* GetClassNonConst(const std::string& className);
		// Retrieves a reflected enum representation from this registry, or nullptr if it does not exist.
		Enum* GetEnumNonConst(const std::string& enumName);
		// Retrieves a reflected global function representation from this registry, or nullptr if it does not exist.
		Function* GetFunctionNonConst(const std::string& functionName);

		bool Serialize(class FileIO& io);

		// Resolve a function pointer.
		bool ResolveFunction(const FunctionRegistration& functionRegistration, Class* reflClass = nullptr);

	public:
		// List of classes defined in this registry.
		std::map<std::string, Class> mClasses;
		// List of enums defined in this registry.
		std::map<std::string, Enum> mEnums;
		// List of global functions defined in this registry.
		std::map<std::string, Function> mFunctions;
	};

	// Returns the reflection registry that describes this program.
	Registry& GetSystemRegistry();

	template <typename ClassType>
	const Class* StaticClass(const Registry& registry = GetSystemRegistry())
	{
		return registry.GetClass(ClassType::CLASS_QUALIFIED_NAME);
	}
}