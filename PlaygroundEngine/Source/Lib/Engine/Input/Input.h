#pragma once

#include "Core/Util/Util.h"

namespace playground
{
	bool InputSystemInit();
	void InputSystemDoFrameLate(float DeltaTime);
	void InputSystemShutdown();
	
	bool IsKeyDown(int keyCode, bool ctrl = false, bool shift = false, bool alt = false);
	bool IsKeyPressed(int keyCode, bool ctrl = false, bool shift = false, bool alt = false);

	// Always returns key state, regardless of any special control filtering.
	bool IsSystemKeyDown(int keyCode, bool ctrl = false, bool shift = false, bool alt = false);
	bool IsSystemKeyPressed(int keyCode, bool ctrl = false, bool shift = false, bool alt = false);
	bool IsSystemKeyReleased(int keyCode, bool ctrl = false, bool shift = false, bool alt = false);

	bool IsShiftDown();
	bool IsControlDown();
	bool IsAltDown();

	Tuple2<int> GetMouseDelta();

	void ForceShowCursor(bool show);
}