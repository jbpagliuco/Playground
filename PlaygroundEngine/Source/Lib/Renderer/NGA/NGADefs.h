#pragma once

#include "Core/Core.h"

#define COM_SAFE_RELEASE(x) if (x != nullptr) { x->Release(); x = nullptr; }