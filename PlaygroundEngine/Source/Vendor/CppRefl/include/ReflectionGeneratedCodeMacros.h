// Use a header guard for the other macros in the file.
// We can't use #pragma once because we need to be able to redefine the GENERATED_REFLECTION_CODE() macro as needed.
#if !defined(GENERATED_CODE_MACROS_H)
#define GENERATED_CODE_MACROS_H

#define REFL_INTERNAL_COMBINE2(A,B,C) A##B##C
#define REFL_INTERNAL_COMBINE(A,B,C) REFL_INTERNAL_COMBINE2(A,B,C)

#define REFL_INTERNAL_TO_STR2(X) #X
#define REFL_INTERNAL_TO_STR(X) REFL_INTERNAL_TO_STR2(X)

#define REFL_INTERNAL_GENERATED_REFLECTION_CODE_MARKER __ReflGeneratedReflectionCodeMarker
#define REFL_INTERNAL_GENERATED_REFLECTION_CODE_MARKER_STR REFL_INTERNAL_TO_STR(REFL_INTERNAL_GENERATED_REFLECTION_CODE_MARKER)

#endif // !defined(GENERATED_CODE_MACROS_H)

// Always redefine this macro when this file is included.
#undef GENERATED_REFLECTION_CODE

#if defined(REFL_BUILD_REFLECTION)
	// Define a special marker so the reflection compiler can find which line this macro is used at.
	#define GENERATED_REFLECTION_CODE() void REFL_INTERNAL_GENERATED_REFLECTION_CODE_MARKER();
#elif defined(REFL_INTERNAL_FILE_ID)
	// Calls a special macro defined in the .reflgen.h file. This uses the line number so we can have multiple reflected classes in one file.
	#define GENERATED_REFLECTION_CODE() REFL_INTERNAL_COMBINE(GENERATED_REFLECTION_CODE_, REFL_INTERNAL_FILE_ID, __LINE__)()
#else
	// Define an empty macro so classes can always have it defined, regardless of whether or not the reflection compiler has been run.
	#define GENERATED_REFLECTION_CODE()
#endif