#include "OSWin32.h"

#if CORE_PLATFORM(PC)

#include <vector>

#include "Debug/Assert.h"

namespace playground
{
	static std::vector<bool(*)(HWND, UINT, WPARAM, LPARAM)> Callbacks;

	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg) {
		case WM_KEYDOWN:
		{
			if (wparam == VK_ESCAPE) {
				PostQuitMessage(0);
			}

			break;
		}

		case WM_DESTROY:
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
		};

		// System callbacks
		bool handled = false;
		for (auto &callback : Callbacks) {
			if (callback(hwnd, msg, wparam, lparam)) {
				handled = true;
			}
		}

		if (handled) {
			return 0;
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	void RegisterWndProcCallback(bool(*callback)(HWND, UINT, WPARAM, LPARAM))
	{
		Callbacks.push_back(callback);
	}

	Window CreateWindowWin32(int x, int y, int w, int h, const wchar_t *title)
	{
		HINSTANCE appInst = GetModuleHandle(NULL);

		// 1. Register our window class
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.lpfnWndProc = MainWndProc;
		wc.hInstance = appInst;
		wc.hIconSm = wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = title;
		wc.style = CS_HREDRAW | CS_VREDRAW;

		CORE_ASSERT_RETURN_VALUE(RegisterClassEx(&wc), INVALID_WINDOW, "Failed to register window class '%ls'. Error Code: %d", title, GetLastError());

		// 2. Create the window
		HWND hwnd = CreateWindowEx(
			NULL,
			title,
			title,
			WS_OVERLAPPEDWINDOW,

			(x == -1) ? CW_USEDEFAULT : x, (y == -1) ? CW_USEDEFAULT : y,
			w, h,

			NULL,
			NULL,
			appInst,
			NULL
		);

		CORE_ASSERT_RETURN_VALUE(hwnd != NULL, INVALID_WINDOW, "Failed to create window '%ls'. Error Code: %d", title, GetLastError());

		// 3. Show the window
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);

		// Success!
		return { x, y, w, h, hwnd };
	}


	bool Window::IsActiveWindow()const
	{
		return GetForegroundWindow() == handle;
	}

	Point Window::GetPosition()const
	{
		RECT rect;
		GetWindowRect(handle, &rect);

		return Point(rect.left, rect.top);
	}

	Size Window::GetSize()const
	{
		RECT rect;
		GetWindowRect(handle, &rect);

		return Size(rect.right - rect.left, rect.bottom - rect.top);
	}

	Size Window::GetViewportSize()const
	{
		RECT rect;
		GetClientRect(handle, &rect);

		// .right and .bottom are 0
		return Size(rect.right, rect.bottom);
	}
}

#endif