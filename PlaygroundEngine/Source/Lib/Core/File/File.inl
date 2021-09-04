namespace playground
{
	template <typename T>
	bool File::Read(T &out)
	{
		return ReadBytes((char*)&out, sizeof(T));
	}

	template <typename T>
	bool File::Write(const T &val)
	{
		return WriteBytes(reinterpret_cast<char*>((void*)&val), sizeof(T));
	}
}