#pragma once

#include <string>
#include <filesystem>

namespace ZenEngine
{

    namespace Filesystem
    {
        std::string ReadFileToString(const std::filesystem::path &inFilepath);

        void WriteBytes(const std::filesystem::path &inFilepath, const uint8_t *inBytes, uint64_t inSize);
    }

}