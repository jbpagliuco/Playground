#pragma once

#include "Core.h"

#if CORE_DEBUG_ENABLE(IMGUI)

#include "imgui/imgui.h"

// I could rewrite this code, but what's the point?
#if CORE_RENDER_API(DX11)
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx11.h"
#elif CORE_RENDER_API(DX12)
#include "imgui/examples/imgui_impl_win32.h"
#include "imgui/examples/imgui_impl_dx12.h"
#endif

#endif