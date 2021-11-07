#include "CRC.h"

#include <stddef.h>
#include <stdint.h>

// Yoink: https://stackoverflow.com/questions/27939882/fast-crc-algorithm

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

namespace playground
{
	Checksum32 crc32(const void* buf, size_t len, Checksum32 crc)
	{
		const unsigned char* cbuf = (const unsigned char*)buf;

		int k;

		crc = ~crc;
		while (len--) {
			crc ^= *cbuf++;
			for (k = 0; k < 8; k++)
				crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
		}
		return ~crc;
	}
}