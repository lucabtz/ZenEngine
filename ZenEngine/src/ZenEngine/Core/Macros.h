#pragma once

#include "Platform.h"
#include "Log.h"

// Debug related macros
#ifdef ZE_DEBUG
	#if defined(ZE_PLATFORM_WINDOWS)
		#define ZE_DEBUGBREAK() __debugbreak()
	#elif defined(ZE_PLATFORM_LINUX)
		#include <signal.h>
		#define ZE_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define ZE_ENABLE_ASSERTS
#else
	#define ZE_DEBUGBREAK()
#endif
// End of debug related macros

// Assertion related macros
#ifdef ZE_ENABLE_ASSERTS
	#define ZE_ASSERT(expression) { if (!(expression)) { ZE_DEBUGBREAK(); } }
	#define ZE_ASSERT_CORE_MSG(expression, ...) { if (!(expression)) { ZE_CORE_CRITICAL(__VA_ARGS__); ZE_DEBUGBREAK(); } }
#else
	#define ZE_ASSERT(expression)
	#define ZE_ASSERT_CORE_MSG(expression, ...)
#endif
// End of assertion related macros

#define BIT(n) (1 << n)

// Entrypoint related macros
#define SET_GAME_CLASS(classname) std::unique_ptr<ZenEngine::Game> CreateGame(const ZenEngine::RuntimeInfo &inRuntimeInfo) { return std::make_unique<classname>(inRuntimeInfo); }
#define IMPLEMENT_GAME_CLASS(classname, gamename) classname(const ZenEngine::RuntimeInfo &inRuntimeInfo) : ZenEngine::Game(gamename, inRuntimeInfo) {}