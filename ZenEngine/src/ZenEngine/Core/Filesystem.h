#pragma once

#include <string>

namespace ZenEngine
{

    namespace Filesystem
    {
        std::string ReadFileToString(const std::string &inFilepath);

        void WriteBytes(const std::string &inFilepath, const uint8_t *inBytes, uint64_t inSize);
    }

}