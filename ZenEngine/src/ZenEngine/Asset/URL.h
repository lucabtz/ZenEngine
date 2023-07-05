#pragma once

#include <string>
#include "Serialization.h"
#include "ZenEngine/Core/Result.h"

namespace ZenEngine
{
    class URL
    {
    public:
        URL() = default;
        URL(const std::string &inScheme, const std::string &inUsername, const std::string &inPassword, const std::string &inPath)
            : mScheme(inScheme), mUsername(inUsername), mPassword(inPassword), mPath(inPath)
        {}
        URL(const URL & inOther)
            : mScheme(inOther.mScheme), mUsername(inOther.mUsername), mPassword(inOther.mPassword), mPath(inOther.mPath)
        {}

        URL &operator =(const URL &inOther)
        {
            mScheme = inOther.mScheme;
            mUsername = inOther.mUsername;
            mPassword = inOther.mPassword;
            mPath = inOther.mPath;
            return *this;
        }

        const std::string &GetScheme() const { return mScheme; }
        const std::string &GetUsername() const { return mUsername; }
        const std::string &GetPassword() const { return mPassword; }
        const std::string &GetPath() const { return mPath; }

        void SetPath(const std::string &inPath) { mPath = inPath; }
    private:
        std::string mScheme;
        std::string mUsername;
        std::string mPassword;
        std::string mPath;

        template <typename Archive>
        void Serialize(Archive &outArchive)
        {
            outArchive(mScheme, mUsername, mPassword, mPath);
        }

        friend class cereal::access;  
    };

    static Result<URL, std::string> ParseURL(const std::string &inURLString);
}