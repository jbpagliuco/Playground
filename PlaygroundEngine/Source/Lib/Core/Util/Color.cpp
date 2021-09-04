#include "Color.h"

#include <string>
#include <map>

#include "Debug/Assert.h"
#include "String.h"
#include "Util.h"

namespace playground
{
	const Color COLOR_BLACK = Color(0, 0, 0);
	const Color COLOR_DARKGREY = Color(64, 64, 64);
	const Color COLOR_GREY = Color(127, 127, 127);
	const Color COLOR_LIGHTGREY = Color(191, 191, 191);
	const Color COLOR_WHITE = Color(255, 255, 255);

	const Color COLOR_RED = Color(255, 0, 0);
	const Color COLOR_MAROON = Color(127, 0, 0);

	const Color COLOR_GREEN = Color(0, 127, 0);
	const Color COLOR_LIME = Color(0, 255, 0);

	const Color COLOR_BLUE = Color(0, 0, 255);
	const Color COLOR_NAVY = Color(0, 0, 127);

	const Color COLOR_PURPLE = Color(127, 0, 127);
	const Color COLOR_TEAL = Color(0, 127, 127);
	const Color COLOR_OLIVE = Color(127, 127, 0);

	const Color COLOR_YELLOW = Color(255, 255, 0);
	const Color COLOR_AQUA = Color(0, 255, 255);
	const Color COLOR_FUCHSIA = Color(255, 0, 255);

	const Color COLOR_CORNFLOWERBLUE = Color(101, 156, 239);

	Color::Color() : r(0), g(0), b(0), a(255) { }
	Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) { }

	Color::Color(const std::string &colorString)
	{
		static std::map<std::string, Color> _colors;
		static bool bInit = false;

		if (!bInit) {
			_colors["black"] = COLOR_BLACK;
			_colors["darkgrey"] = COLOR_DARKGREY;
			_colors["grey"] = COLOR_GREY;
			_colors["lightgrey"] = COLOR_LIGHTGREY;
			_colors["white"] = COLOR_WHITE;
			_colors["red"] = COLOR_RED;
			_colors["maroon"] = COLOR_MAROON;
			_colors["green"] = COLOR_GREEN;
			_colors["lime"] = COLOR_LIME;
			_colors["blue"] = COLOR_BLUE;
			_colors["navy"] = COLOR_NAVY;
			_colors["purple"] = COLOR_PURPLE;
			_colors["teal"] = COLOR_TEAL;
			_colors["olive"] = COLOR_OLIVE;
			_colors["yellow"] = COLOR_YELLOW;
			_colors["aqua"] = COLOR_AQUA;
			_colors["fuchsia"] = COLOR_FUCHSIA;
			_colors["cornflowerblue"] = COLOR_CORNFLOWERBLUE;
			bInit = true;
		}

		char colorStringLower[32];
		strcpy_s(colorStringLower, colorString.c_str());
		StrToLowerCase(colorStringLower);

		if (_colors.find(colorStringLower) != _colors.end())
			*this = _colors[colorStringLower];
		else {
			CORE_ASSERT(false, "Color string '%s' was not found. Defaulting to red.", colorStringLower);
			*this = COLOR_BLACK;
		}
	}

	uint32_t Color::_R8G8B8()const
	{
		return (0x00 << 24) | (r << 16) | (g << 8) | (b << 0);
	}
	uint32_t Color::_B8G8R8()const
	{
		return (0x00 << 24) | (b << 16) | (g << 8) | (r << 0);
	}
	uint32_t Color::R8G8B8_()const
	{
		return (r << 24) | (g << 16) | (b << 8) | (0x00);
	}
	uint32_t Color::B8G8R8_()const
	{
		return (b << 24) | (g << 16) | (r << 8) | (0x00);
	}

	uint32_t Color::R8G8B8A8()const
	{
		return (r << 24) | (g << 16) | (b << 8) | (a << 0);
	}
	uint32_t Color::B8G8R8A8()const
	{
		return (b << 24) | (g << 16) | (r << 8) | (a << 0);
	}
	uint32_t Color::A8R8G8B8()const
	{
		return (a << 24) | (r << 16) | (g << 8) | (b << 0);
	}
	uint32_t Color::A8B8G8R8()const
	{
		return (a << 24) | (b << 16) | (g << 8) | (r << 0);
	}

#if CORE_PLATFORM(PC)
	uint32_t Color::toWin32()const
	{
		return _B8G8R8();
	}
#endif

	ColorF::ColorF() : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
	{
	}

	ColorF::ColorF(float r, float g, float b, float a /* = 1.0f */) : r(r), g(g), b(b), a(a)
	{
		Clamp(r, 0.0f, 1.0f);
		Clamp(g, 0.0f, 1.0f);
		Clamp(b, 0.0f, 1.0f);
		Clamp(a, 0.0f, 1.0f);
	}

	ColorF::ColorF(const Color &color) :
		r((float)color.r / 255.0f),
		g((float)color.g / 255.0f),
		b((float)color.b / 255.0f),
		a((float)color.a / 255.0f)
	{
	}

	ColorF ColorF::operator+(const ColorF &rhs)const
	{
		return ColorF(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}

	ColorF& ColorF::operator+=(const ColorF &rhs)
	{
		Clamp((r += rhs.r), 0.0f, 1.0f);
		Clamp((g += rhs.g), 0.0f, 1.0f);
		Clamp((b += rhs.b), 0.0f, 1.0f);
		Clamp((a += rhs.a), 0.0f, 1.0f);

		return *this;
	}

	ColorF ColorF::operator-(const ColorF &rhs)const
	{
		return ColorF(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
	}

	ColorF& ColorF::operator-=(const ColorF &rhs)
	{
		Clamp((r -= rhs.r), 0.0f, 1.0f);
		Clamp((g -= rhs.g), 0.0f, 1.0f);
		Clamp((b -= rhs.b), 0.0f, 1.0f);
		Clamp((a -= rhs.a), 0.0f, 1.0f);

		return *this;
	}
}