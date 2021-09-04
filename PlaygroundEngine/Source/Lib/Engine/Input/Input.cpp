#include "Input.h"

#include "Core/OS/OS.h"
#include "Core/Util/Util.h"
#include "Renderer/ImguiRenderer.h"
#include "Renderer/Renderer.h"

#if CORE_PLATFORM(PC)
#include "Core/OS/OSWin32.h"
#endif

#define MODIFIER_KEY(flag, state) (!(flag) || state)

namespace playground
{
	struct KeyState {
		bool mDown = false;
		bool mPressed = false;
		bool mReleased = false;
	};
	static KeyState KeyStates[0xff];

	static Tuple2<int> MouseDelta = { 0, 0 };
	
	static bool InputEnabled = true;

	
	void ForceShowCursor(bool show)
	{
		if (show) {
			while (ShowCursor(true) < 0);
		} else {
			while (ShowCursor(false) >= 0);
		}
	}

	static void SetFocus(bool focus)
	{
		InputEnabled = focus;
		ForceShowCursor(!focus);
	}

	static void OnKeyDownCallback(int keyCode)
	{
		// Key is down
		KeyStates[keyCode].mDown = true;

		// Signal that this key was just pressed. It will be cleared at the end of the frame.
		KeyStates[keyCode].mPressed = true;
	}

	static void OnKeyUpCallback(int keyCode)
	{
		// Key is up
		KeyStates[keyCode].mDown = false;

		// Signal that this key was just release. It will be cleared at the end of the frame.
		KeyStates[keyCode].mReleased = true;
	}

	static void OnMouseMoveCallback(int dx, int dy)
	{
		MouseDelta = { dx, dy };
	}

	static bool InputWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg) {
		case WM_KEYDOWN:
		{
			if (InputEnabled) {
				OnKeyDownCallback((int)wparam);
				return true;
			}
		}

		case WM_KEYUP:
		{
			if (InputEnabled) {
				OnKeyUpCallback((int)wparam);
				return true;
			}
		}

		case WM_INPUT:
		{
			if (InputEnabled) {
				UINT dwSize = sizeof(RAWINPUT);
				static BYTE lpb[sizeof(RAWINPUT)];

				GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
				RAWINPUT *raw = (RAWINPUT*)lpb;

				if (raw->header.dwType == RIM_TYPEMOUSE) {
					// OnMouseMoveCallback(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}

				return false;
			}
		}

		case WM_ACTIVATE:
			SetFocus(Playground_Renderer->GetWindow().IsActiveWindow());
			return true;
		};

		return false;
	}





	bool InputSystemInit()
	{
		for (int i = 0; i < 0xFF; ++i) {
			KeyStates[i] = KeyState();
		}

		const USHORT HID_USAGE_PAGE_GENERIC = 0x01;
		const USHORT HID_USAGE_GENERIC_MOUSE = 0x02;

		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags = RIDEV_INPUTSINK;
		rid[0].hwndTarget = Playground_Renderer->GetWindow().handle;
		const BOOL registered = RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
		CORE_ASSERT(registered, "Failed to register mouse input device.");

#if !CORE_BUILD_TYPE(TOOLS)
		// Hide the mouse cursor
		while (ShowCursor(false) > 0);
#endif

		RegisterWndProcCallback(&InputWndProc);

		return true;
	}

	void InputSystemDoFrameLate(float DeltaTime)
	{
		MouseDelta = { 0, 0 };

		if (InputEnabled && !ImguiRendererGetFocus()) {
			const Window &window = Playground_Renderer->GetWindow();
			const playground::Point windowPos = window.GetPosition();
			const playground::Size windowSize = window.GetViewportSize();

			const playground::Point newPosition(windowPos.x + windowSize.x / 2, windowPos.y + windowSize.y / 2);

			POINT oldCursorPosition;
			GetCursorPos(&oldCursorPosition);
			
			MouseDelta = { oldCursorPosition.x - newPosition.x, oldCursorPosition.y - newPosition.y };

			SetCursorPos(newPosition.x, newPosition.y);
		}

		for (int i = 0; i < 0xFF; ++i) {
			KeyStates[i].mPressed = false;
			KeyStates[i].mReleased = false;
		}
	}

	void InputSystemShutdown()
	{

	}

	bool IsKeyDown(int keyCode, bool ctrl, bool shift, bool alt)
	{
		if (ImguiRendererGetFocus()) {
			return false;
		}

		return IsSystemKeyDown(keyCode, ctrl, shift, alt);
	}

	bool IsKeyPressed(int keyCode, bool ctrl, bool shift, bool alt)
	{
		if (ImguiRendererGetFocus()) {
			return false;
		}
		
		return IsSystemKeyPressed(keyCode);
	}

	bool IsSystemKeyDown(int keyCode, bool ctrl, bool shift, bool alt)
	{
		return KeyStates[keyCode].mDown &&
			MODIFIER_KEY(ctrl, KeyStates[VK_CONTROL].mDown) &&
			MODIFIER_KEY(shift, KeyStates[VK_SHIFT].mDown) &&
			MODIFIER_KEY(alt, KeyStates[VK_MENU].mDown);
	}


	bool IsSystemKeyPressed(int keyCode, bool ctrl, bool shift, bool alt)
	{
		return KeyStates[keyCode].mPressed &&
			MODIFIER_KEY(ctrl, KeyStates[VK_CONTROL].mDown) &&
			MODIFIER_KEY(shift, KeyStates[VK_SHIFT].mDown) &&
			MODIFIER_KEY(alt, KeyStates[VK_MENU].mDown);
	}

	bool IsSystemKeyReleased(int keyCode, bool ctrl, bool shift, bool alt)
	{
		return KeyStates[keyCode].mReleased &&
			MODIFIER_KEY(ctrl, KeyStates[VK_CONTROL].mDown) &&
			MODIFIER_KEY(shift, KeyStates[VK_SHIFT].mDown) &&
			MODIFIER_KEY(alt, KeyStates[VK_MENU].mDown);
	}

	bool IsShiftDown()
	{
		return KeyStates[VK_SHIFT].mDown;
	}

	bool IsControlDown()
	{
		return KeyStates[VK_CONTROL].mDown;
	}

	bool IsAltDown()
	{
		return KeyStates[VK_MENU].mDown;
	}

	Tuple2<int> GetMouseDelta()
	{
		if (ImguiRendererGetFocus()) {
			return Tuple2<int>(0, 0);
		}

		return MouseDelta;
	}
}