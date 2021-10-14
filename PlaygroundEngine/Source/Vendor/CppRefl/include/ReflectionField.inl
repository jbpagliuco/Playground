#include "ReflectionErrorHandling.h"

namespace refl
{
	template <typename T>
	T* Field::GetDataPtr(void* obj)const
	{
		return static_cast<T*>(GetRawDataPtr(obj));
	}

	template <typename T>
	T* Field::GetArrayElement(void* obj, int index)const
	{
		if (!mTypeInfo.IsFixedArray()) {
			REFL_INTERNAL_RAISE_ERROR("Tried to get array element from a non-array type.");
			return nullptr;
		}

		if (index < 0 || index >= mTypeInfo.mArraySize) {
			REFL_INTERNAL_RAISE_ERROR("Tried to get out-of-bounds array element (%d).", index);
			return nullptr;
		}

		T* arrayStart = GetDataPtr<T>(obj);
		return arrayStart + index;
	}
}