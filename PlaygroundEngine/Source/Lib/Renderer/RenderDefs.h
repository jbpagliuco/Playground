#pragma once

#include "Core/Core.h"
#include "Core/Debug/Assert.h"

#define RENDER_ASSERT_RETURN(cond, ...)				CORE_ASSERT_RETURN_CLEANUP(cond, Shutdown, __VA_ARGS__)
#define RENDER_ASSERT_RETURN_VALUE(cond, rv, ...)	CORE_ASSERT_RETURN_VALUE_CLEANUP(cond, rv, Shutdown, __VA_ARGS__)

// Features
#define RENDER_FEATURE(X)						RENDER_FEATURE_PRIVATE_##X()
#define RENDER_FEATURE_PRIVATE_SHADOWS()		0

// Debug features
#define RENDER_DEBUG_FEATURE(X)						RENDER_DEBUG_FEATURE_PRIVATE_##X()
#define RENDER_DEBUG_FEATURE_PRIVATE_STORE_NAMES()	CORE_CONFIG(DEBUG)