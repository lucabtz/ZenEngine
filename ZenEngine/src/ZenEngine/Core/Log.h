#pragma once


#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include "Macros.h"

namespace ZenEngine
{

    class Log
    {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger> & GetCoreLogger() { return sCoreLogger; }
        static std::shared_ptr<spdlog::logger> & GetGameLogger() { return sGameLogger; }

    private:
        static std::shared_ptr<spdlog::logger> sCoreLogger;
        static std::shared_ptr<spdlog::logger> sGameLogger;
    };

}

// Logging related macros
#define ZE_CORE_TRACE(...)    ::ZenEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ZE_CORE_INFO(...)     ::ZenEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ZE_CORE_WARN(...)     ::ZenEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ZE_CORE_ERROR(...)    ::ZenEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ZE_CORE_CRITICAL(...) ::ZenEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define ZE_TRACE(...)         ::ZenEngine::Log::GetGameLogger()->trace(__VA_ARGS__)
#define ZE_INFO(...)          ::ZenEngine::Log::GetGameLogger()->info(__VA_ARGS__)
#define ZE_WARN(...)          ::ZenEngine::Log::GetGameLogger()->warn(__VA_ARGS__)
#define ZE_ERROR(...)         ::ZenEngine::Log::GetGameLogger()->error(__VA_ARGS__)
#define ZE_CRITICAL(...)      ::ZenEngine::Log::GetGameLogger()->critical(__VA_ARGS__)

#include <cstdlib>
#ifdef ZE_DEBUG
    #define ZE_CORE_DIE(...) { ZE_CORE_CRITICAL(__VA_ARGS__); ZE_DEBUGBREAK(); }
#else
    #define ZE_CORE_DIE(...) { ZE_CORE_CRITICAL(__VA_ARGS__); std::abort(); }
#endif