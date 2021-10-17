#pragma once

#include <optional>

#include "ReflectionElement.h"
#include "./Private/FunctionInvocation.h"
#include "./Private/FunctionRegistration.h"

namespace refl
{
	// Represents a reflected function.
	class Function : public Element
	{
	public:
		using Element::DeepEquals;

		// Checks for equality among every aspect of this element.
		// NB: This is really only necessary for testing purposes.
		virtual bool DeepEquals(const Function& rhs)const;

		// Read or write this element.
		virtual void Serialize(class FileIO& io, const class Registry& registry) override;

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Global functions

		// Invoke a reflected global function.
		template <typename... ParamTypes>
		void InvokeGlobal(ParamTypes&&... params)const;

		// Invoke a reflected global function that returns a value.
		// NB: If you invoke this function, you must explicitly specify all parameter template arguments.
		template <typename ReturnType, typename... ParamTypes>
		std::optional<ReturnType> InvokeGlobal(ParamTypes&&... params)const;

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// Member functions

		// Invoke a reflected member function.
		template <typename... ParamTypes>
		void InvokeMember(void* obj, ParamTypes&&... params)const;

		// Invoke a reflected member function that returns a value.
		template <typename ReturnType, typename... ParamTypes>
		std::optional<ReturnType> InvokeMember(void* obj, ParamTypes&&... params)const;

		// Invoke a reflected member function.
		template <typename ObjectType, typename... ParamTypes>
		void InvokeMember(ObjectType& obj, ParamTypes&&... params)const;

		// Invoke a reflected member function that returns a value.
		template <typename ReturnType, typename ObjectType, typename... ParamTypes>
		std::optional<ReturnType> InvokeMember(ObjectType& obj, ParamTypes&&... params)const;

		//////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(REFL_DEBUG)
		// Creates a string representation of this class.
		virtual std::string ToString(int indent = 0)const override;
#endif

	private:
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// These two functions actually invoke the real function.
		void InvokeInternal(void* obj_or_null, void* param1_or_null = nullptr)const;

		template <typename ReturnType>
		std::optional<ReturnType> InvokeInternal(void* obj_or_null, void* param1_or_null = nullptr)const;
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		// Check if we can safely invoke a function with the given parameters.
		bool ValidateInvocation(void* obj, void* param1)const;

	public:
		// Return value type of this function (only primitive data types are supported).
		DataType mReturnType = DataType::VOID_TYPE;

		// Parameter type information.
		std::vector<TypeInstance> mParameterTypes;

		// Is this a member function or a global function?
		bool mIsMemberFunction;

		// Function pointer
		FunctionType mFunction = nullptr;
		void* mFunctionInvoker = nullptr;
	};
}