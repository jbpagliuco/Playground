#pragma once

#include <stdint.h>

namespace playground
{
	typedef uint32_t Checksum32;

	// Computes a 32-bit checksum of the given data.
	Checksum32 crc32(const void* buf, size_t len, Checksum32 crc = 0);

	template <typename T>
	Checksum32 crc32(const T& data, Checksum32 crc = 0)
	{
		return crc32(&data, sizeof(T), crc);
	}
}