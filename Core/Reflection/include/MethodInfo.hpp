#pragma once
#include "Arg.hpp"
#include "MemberInfo.hpp"
#include "MemberTypes.hpp"
#include <any>
#include <array>
#include <functional>
#include <iostream>
#include <tuple>
#include <type_traits>

namespace MReflection
{

class MethodInfo final : public MemberInfo
{
    using InvokeHandler = std::function<std::any(std::any, std::any)>;

  public:
    template <class TClass, typename TRet, typename... TArgs>
    MethodInfo(const std::string &name, TRet (TClass::*methodPointer)(TArgs...), TypeInfo *declaringType)
        : MemberInfo(name, declaringType)
    {
        mInvokeHandler = [methodPointer](std::any obj, std::any args) -> std::any {
            auto &instance = std::any_cast<std::reference_wrapper<TClass>>(obj).get();
            auto &argsArray = std::any_cast<std::reference_wrapper<std::array<Arg, sizeof...(TArgs)>>>(args).get();
            auto tupleArgs = Arg::ToTuple<TArgs...>(argsArray);
            return std::apply(
                [&instance, methodPointer](auto &&...unpackedArgs) {
                    if constexpr (std::is_void_v<TRet>)
                    {
                        (instance.*methodPointer)(std::forward<TArgs>(unpackedArgs)...);
                        return std::any{};
                    }
                    else
                    {
                        return (instance.*methodPointer)(std::forward<TArgs>(unpackedArgs)...);
                    }
                },
                tupleArgs);
        };
        mIsConst = false;
    }
    template <class TClass, typename TRet, typename... TArgs>
    MethodInfo(const std::string &name, TRet (TClass::*methodPointer)(TArgs...) const, TypeInfo *declaringType)
        : MemberInfo(name, declaringType)
    {
        mInvokeHandler = [methodPointer](std::any obj, std::any args) -> std::any {
            auto &instance = std::any_cast<std::reference_wrapper<const TClass>>(obj).get();
            auto &argsArray = std::any_cast<std::reference_wrapper<std::array<Arg, sizeof...(TArgs)>>>(args).get();
            auto tupleArgs = Arg::ToTuple<TArgs...>(argsArray);
            return std::apply(
                [&instance, methodPointer](auto &&...unpackedArgs) {
                    if constexpr (std::is_void_v<TRet>)
                    {
                        (instance.*methodPointer)(std::forward<TArgs>(unpackedArgs)...);
                        return std::any{};
                    }
                    else
                    {
                        return (instance.*methodPointer)(std::forward<TArgs>(unpackedArgs)...);
                    }
                },
                tupleArgs);
        };
        mIsConst = true;
    }
    inline MemberTypes GetMemberType() const override
    {
        return MemberTypes::Method;
    }

  public:
    template <class TClass, typename... TArgs> std::any Invoke(TClass &obj, TArgs &&...args) const
    {
        std::array<Arg, sizeof...(TArgs)> argArray{Arg(std::forward<TArgs>(args))...};
        if (mIsConst)
        {
            return mInvokeHandler(std::cref(obj), std::ref(argArray));
        }
        return mInvokeHandler(std::ref(obj), std::ref(argArray));
    }

  private:
    bool mIsConst{false};
    bool mIsStatic{false};
    bool mIsVirtual{false};
    bool mIsPublic{true};
    bool mIsProtected{false};
    bool mIsPrivate{false};
    InvokeHandler mInvokeHandler{nullptr};
};
} // namespace MReflection