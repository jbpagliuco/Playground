#pragma once

#define PLAYGROUND_PHYSICS_LOG_FILTER "Physics"

#define PLAYGROUND_PHYSICS_SAFE_RELEASE(x) if ((x)) { (x)->release(); (x) = nullptr; }