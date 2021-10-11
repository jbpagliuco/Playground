
// Global switch to disable generated reflected code.
#if !defined(REFL_GEN_DISABLE)

// This must be defined before we include the private macros so GENERATED_REFLECTION_CODE() works correctly.
#define REFL_INTERNAL_FILE_ID CameraComponent

#if defined(CameraComponent_REFLGEN_H)
#error Including CameraComponent.reflgen.h multiple times! Use `#pragma once` in CameraComponent.h.
#endif
#define CameraComponent_REFLGEN_H

#include "ReflectionRegistry.h"
#include "ReflectionGeneratedCodeMacros.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CameraComponent

#if !defined(REFL_BUILD_REFLECTION)
// Macro to be added inside the definition of a reflected class.
#define GENERATED_REFLECTION_CODE_CameraComponent16()															\
	public:																													\
		static constexpr const char* CLASS_NAME				= "CameraComponent";											\
		static constexpr const char* CLASS_QUALIFIED_NAME	= "playground::CameraComponent";									\
																															\
		/* Called by generated code to hook up runtime specific data */														\
		static void __ReflRegisterClass(class refl::Registry* registry);													\
																															\
		static inline const refl::Class& StaticClass() { return *refl::StaticClass<CameraComponent>(); }					\
																															\
	private:

#else // !defined(REFL_BUILD_REFLECTION)

// Define empty macro when building reflection.
#define GENERATED_REFLECTION_CODE_CameraComponent16()

#endif // !defined(REFL_BUILD_REFLECTION)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // !defined(REFL_GEN_DISABLE)
