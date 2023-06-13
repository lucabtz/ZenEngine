#include "Time.h"

#include <chrono>

namespace ZenEngine
{
    uint64_t Time::GetTimeMicroseconds()
    {
        using namespace std::chrono;
        return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    }

    double Time::GetTime()
    {
        return (((double)GetTimeMicroseconds())/(double)1000.0);
    }
}