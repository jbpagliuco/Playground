#pragma once

#include <stdint.h>

template <size_t S>
struct _CORE_ENUM_FLAG_INTEGER_FOR_SIZE;

template <> struct _CORE_ENUM_FLAG_INTEGER_FOR_SIZE<1> { typedef uint8_t type; };
template <> struct _CORE_ENUM_FLAG_INTEGER_FOR_SIZE<2> { typedef uint16_t type; };
template <> struct _CORE_ENUM_FLAG_INTEGER_FOR_SIZE<4> { typedef uint32_t type; };
template <> struct _CORE_ENUM_FLAG_INTEGER_FOR_SIZE<8> { typedef uint64_t type; };

template <class T>
struct _CORE_ENUM_FLAG_SIZED_INTEGER
{
	typedef typename _CORE_ENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
};

#define CORE_DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE)																																						\
	inline ENUMTYPE  operator |  (ENUMTYPE a, ENUMTYPE b)   { return (ENUMTYPE)  (((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)   |  ((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); }		\
	inline ENUMTYPE& operator |= (ENUMTYPE &a, ENUMTYPE b)  { return (ENUMTYPE&) (((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) |= ((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); }		\
	inline ENUMTYPE  operator &  (ENUMTYPE a, ENUMTYPE b)   { return (ENUMTYPE)  (((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)   &  ((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); }		\
	inline ENUMTYPE& operator &= (ENUMTYPE &a, ENUMTYPE b)  { return (ENUMTYPE&) (((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) &= ((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); }		\
	inline ENUMTYPE  operator ^  (ENUMTYPE a, ENUMTYPE b)   { return (ENUMTYPE)  (((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)   ^  ((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); }		\
	inline ENUMTYPE& operator ^= (ENUMTYPE &a, ENUMTYPE b)  { return (ENUMTYPE&) (((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) ^= ((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); }		\
	inline ENUMTYPE  operator ~  (ENUMTYPE a)               { return (ENUMTYPE)  (~((_CORE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); }