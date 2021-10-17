#include "ReflectionErrorHandling.h"

namespace refl
{
	template <typename T>
	T* Field::GetDataPtr(void* obj)const
	{
		return static_cast<T*>(GetDataPtr(obj));
	}

	template <typename T>
	T* Field::GetArrayElement(void* obj, int index)const
	{
		return static_cast<T*>(GetArrayElement(obj, index));
	}
}