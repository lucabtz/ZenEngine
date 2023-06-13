#pragma once

#include <stdint.h>

namespace ZenEngine
{
    namespace Time
    {
        /// @brief Returns the current time in microseconds
        /// @return the current time in microseconds
        uint64_t GetTimeMicroseconds();

        /// @brief Returns the current time in milliseconds
        /// @return the current time in milliseconds
        double GetTime();
    }
}