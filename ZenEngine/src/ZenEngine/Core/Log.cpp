#include "Log.h"

#include <memory>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "Macros.h"

namespace ZenEngine
{
    std::shared_ptr<spdlog::logger> Log::sCoreLogger;
    std::shared_ptr<spdlog::logger> Log::sGameLogger;


    void Log::Init()
    {
        std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("ZenEngine.log", true));

        logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");
    
        sCoreLogger = std::make_shared<spdlog::logger>("ZENCORE", begin(logSinks), end(logSinks));
		spdlog::register_logger(sCoreLogger);
		sCoreLogger->set_level(spdlog::level::trace);
		sCoreLogger->flush_on(spdlog::level::trace);

        sGameLogger = std::make_shared<spdlog::logger>("ZENGAME", begin(logSinks), end(logSinks));
		spdlog::register_logger(sGameLogger);
		sGameLogger->set_level(spdlog::level::trace);
		sGameLogger->flush_on(spdlog::level::trace);

        ZE_CORE_INFO("Logger started");
    }
    
}