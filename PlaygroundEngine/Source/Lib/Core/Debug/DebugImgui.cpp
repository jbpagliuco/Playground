#include "DebugImgui.h"

#if CORE_DEBUG_ENABLE(IMGUI)

#include "imgui/imgui.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_widgets.cpp"

#if CORE_RENDER_API(DX11)
#include "imgui/examples/imgui_impl_win32.cpp"
#include "imgui/examples/imgui_impl_dx11.cpp"
#elif CORE_RENDER_API(DX12)
#include "imgui/examples/imgui_impl_win32.cpp"
#include "imgui/examples/imgui_impl_dx12.cpp"
#endif

#endif