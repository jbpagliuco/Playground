#include "OS.h"

#if CORE_PLATFORM(PC)
#include "OSWin32.h"
#endif

#include "Debug/Assert.h"

namespace playground
{
	Window INVALID_WINDOW = { 0, 0, 0, 0, NULL };

	float Window::GetAspectRatio()const
	{
		return (float)width / (float)height;
	}

	Window CreateAndShowWindow(int x, int y, int w, int h, const wchar_t *title)
	{
#if CORE_PLATFORM(PC)
		return CreateWindowWin32(x, y, w, h, title);
#endif
	}
}