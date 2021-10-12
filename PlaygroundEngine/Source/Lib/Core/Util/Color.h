#pragma once

#include <stdint.h>
#include <string>

#include "Core/Core.h"
#include "Core/Reflection/ReflMarkup.h"

#include "Color.reflgen.h"

namespace playground
{
	struct Color
	{
	public:
		uint8_t r, g, b, a;

		Color();
		Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255);
		Color(const std::string &colorString);

		// 0x00RRGGBB
		uint32_t _R8G8B8()const;
		// 0x00BBGGRR
		uint32_t _B8G8R8()const;
		// 0xRRGGBB00
		uint32_t R8G8B8_()const;
		// 0xBBGGRR00
		uint32_t B8G8R8_()const;

		uint32_t R8G8B8A8()const;
		uint32_t B8G8R8A8()const;
		uint32_t A8R8G8B8()const;
		uint32_t A8B8G8R8()const;

#if CORE_PLATFORM(PC)
		uint32_t toWin32()const;
#endif
	};

	extern const Color COLOR_BLACK;
	extern const Color COLOR_DARKGREY;
	extern const Color COLOR_GREY;
	extern const Color COLOR_LIGHTGREY;
	extern const Color COLOR_WHITE;

	extern const Color COLOR_RED;
	extern const Color COLOR_MAROON;

	extern const Color COLOR_GREEN;
	extern const Color COLOR_LIME;

	extern const Color COLOR_BLUE;
	extern const Color COLOR_NAVY;

	extern const Color COLOR_PURPLE;
	extern const Color COLOR_TEAL;
	extern const Color COLOR_OLIVE;

	extern const Color COLOR_YELLOW;
	extern const Color COLOR_AQUA;
	extern const Color COLOR_FUCHSIA;

	extern const Color COLOR_CORNFLOWERBLUE;

	struct REFLECTED ColorF
	{
		GENERATED_REFLECTION_CODE();

	public:
		float r REFLECTED;
		float g REFLECTED;
		float b REFLECTED;
		float a REFLECTED;

		ColorF();
		ColorF(float r, float g, float b, float a = 1.0f);
		ColorF(const Color &color);

		ColorF operator+(const ColorF &rhs)const;
		ColorF& operator+=(const ColorF &rhs);

		ColorF operator-(const ColorF &rhs)const;
		ColorF& operator-=(const ColorF &rhs);

		float* FloatArray();
	};
}