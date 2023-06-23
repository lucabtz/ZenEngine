#include "Filesystem.h"

#include <fstream>
#include <filesystem>

namespace ZenEngine
{

    std::string Filesystem::ReadFileToString(const std::filesystem::path &inFilepath)
    {
        std::ifstream ifs(inFilepath);
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return content;
    }

    void Filesystem::WriteBytes(const std::filesystem::path &inFilepath, const uint8_t *inBytes, uint64_t inSize)
    {
        std::ofstream out(inFilepath, std::ios::out | std::ios::binary);
        if (out.is_open())
        {
            out.write((const char *)inBytes, inSize);
        }
    }
}
