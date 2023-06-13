#include "Filesystem.h"

#include <fstream>

namespace ZenEngine
{

    std::string Filesystem::ReadFileToString(const std::string &inFilename)
    {
        std::ifstream ifs(inFilename);
        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return content;
    }

    void Filesystem::WriteBytes(const std::string &inFilepath, const uint8_t *inBytes, uint64_t inSize)
    {
        std::ofstream out(inFilepath, std::ios::out | std::ios::binary);
        if (out.is_open())
        {
            out.write((const char *)inBytes, inSize);
        }
    }
}
