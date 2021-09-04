#pragma once

#define CORE_CREATE_SINGLETON_INSTANCE(T) T* T::msInst = nullptr; static T _T_Inst
#define CORE_CREATE_SINGLETON_INSTANCE_SUB(T, SubT) T* T::msInst = nullptr; static SubT _SubT_Inst

namespace playground
{
	template <typename T>
	class Singleton
	{
	public:
		Singleton()
		{
			msInst = static_cast<T*>(this);
		}

		static T* Get()
		{
			return msInst;
		}

	protected:
		static T *msInst;
	};
}