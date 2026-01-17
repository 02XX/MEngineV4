#pragma once
#include "MemberInfo.hpp"
#include "MemberTypes.hpp"
#include <any>
#include <functional>
#include <memory>
#include <tuple>

namespace MReflection
{
class ConstructorInfo : public MemberInfo
{
  protected:
    virtual std::any InvokeImpl(std::any args) const = 0;

  public:
    ConstructorInfo(const std::string &name, TypeInfo *declaringType) : MemberInfo(name, declaringType)
    {
    }
    template <class TClass, typename... TArgs> std::shared_ptr<TClass> Create(TArgs &&...args) const
    {
        auto tupleArgs = std::make_tuple(std::forward<TArgs>(args)...);
        return std::any_cast<std::shared_ptr<TClass>>(InvokeImpl(std::move(tupleArgs)));
    }
    virtual ~ConstructorInfo() = default;
    inline MemberTypes GetMemberType() const override
    {
        return MemberTypes::Constructor;
    }
};

template <class TClass, typename... TArgs> class ConstructorInfoImpl : public ConstructorInfo
{
    using InvokeHandler = std::function<std::any(std::any, std::any)>;

  public:
    ConstructorInfoImpl(const std::string &name, TypeInfo *declaringType) : ConstructorInfo(name, declaringType)
    {
    }
    std::any InvokeImpl(std::any args) const override
    {
        auto argsTuple = std::any_cast<std::tuple<TArgs...>>(args);
        return std::apply(
            [](TArgs &&...unpackedArgs) { return std::make_shared<TClass>(std::forward<TArgs>(unpackedArgs)...); },
            std::move(argsTuple));
    }

  private:
    InvokeHandler mInvokeHandler;
};
} // namespace MReflection