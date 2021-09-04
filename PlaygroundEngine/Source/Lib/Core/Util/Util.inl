namespace playground
{
	template <typename T>
	T Min(const T& v1, const T& v2)
	{
		return (v1 < v2) ? v1 : v2;
	}

	template <typename T>
	T& MinRef(T& v1, T& v2)
	{
		return (v1 < v2) ? v1 : v2;
	}

	template <typename T>
	T Max(const T& v1, const T& v2)
	{
		return (v1 < v2) ? v2 : v1;
	}

	template <typename T>
	T& MaxRef(T& v1, T& v2)
	{
		return (v1 < v2) ? v2 : v1;
	}

	template <typename T>
	T& Clamp(T& val, const T& min, const T& max)
	{
		val = Max(min, Min(val, max));
		return val;
	}
}