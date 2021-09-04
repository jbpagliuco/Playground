#pragma once

#include "Core/Core.h"

#if CORE_PLATFORM(PC)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "OS.h"

namespace playground
{
	void RegisterWndProcCallback(bool(*callback)(HWND, UINT, WPARAM, LPARAM));

	Window CreateWindowWin32(int x, int y, int w, int h, const wchar_t *title);
}

#endif