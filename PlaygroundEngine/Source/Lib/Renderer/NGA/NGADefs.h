#pragma once

#include "Core/Core.h"

#include "RenderDefs.h"

#define COM_SAFE_RELEASE(x) if (x != nullptr) { x->Release(); x = nullptr; }