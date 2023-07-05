#pragma once

#include "Macros.h"
#include <functional>
#include <variant>

namespace ZenEngine
{

    template <typename T, typename E>
    requires std::is_default_constructible_v<T>
    class Result
    {
    public:
        Result() : mResult(T()) {}
        Result(const T &inValue) : mResult(inValue) {}
        Result(const E &inError) : mResult(inError) {}
        Result(const Result<T, T> &inOther) : mResult(inOther.mResult) {}
        Result<T, E> &operator =(const Result<T, E> &inOther)
        {
            mResult = inOther.mResult;
            return *this;
        }

        Result<T, E> &operator =(const T& inValue)
        {
            mResult = inValue;
            return *this;
        }

        Result<T, E> &operator =(const E& inError)
        {
            mResult = inError;
            return *this;
        }

        bool IsError()
        {
            return std::holds_alternative<E>(mResult);
        }

        E &GetError()
        {
            return std::get<E>(mResult);
        }

        template <typename Func>
        auto Then(Func inFunction) -> Result<decltype(inFunction(T())), E>
        {
            if (std::holds_alternative<E>(mResult)) return std::get<E>(mResult);
            return inFunction(std::get<T>(mResult));
        }

        T &Unwrap()
        {
            try 
            {
                return std::get<T>(mResult);
            }
            catch (const std::bad_variant_access &e)
            {
                ZE_CORE_DIE("Unwrap!");
            }
        }

    private:
        std::variant<T, E> mResult;
    };
}