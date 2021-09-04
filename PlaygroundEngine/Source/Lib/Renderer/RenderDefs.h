#pragma once

#include "Core/Debug/Assert.h"

#define RENDER_ASSERT_RETURN(cond, ...)				CORE_ASSERT_RETURN_CLEANUP(cond, Shutdown, __VA_ARGS__)
#define RENDER_ASSERT_RETURN_VALUE(cond, rv, ...)	CORE_ASSERT_RETURN_VALUE_CLEANUP(cond, rv, Shutdown, __VA_ARGS__)