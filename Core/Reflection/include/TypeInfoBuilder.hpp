#pragma once
#include "ConstructorInfo.hpp"
#include "TypeInfo.hpp"
#include <memory>

namespace MReflection
{
class TypeInfoBuilder
{
  public:
    TypeInfoBuilder(std::string typeName) : mTypeInfo(std::make_unique<TypeInfo>(typeName))
    {
    }
    virtual ~TypeInfoBuilder();
    virtual TypeInfoBuilder &AddBaseClass(const std::string &baseTClassName);
    virtual TypeInfoBuilder &AddBaseClass(TypeInfo *baseTypeInfo);

    template <class TClass, typename... TArgs> TypeInfoBuilder &AddConstructor(const std::string &constructorName)
    {
        mTypeInfo->mConstructors[constructorName] =
            std::make_unique<ConstructorInfoImpl<TClass, TArgs...>>(constructorName, mTypeInfo.get());
        return *this;
    }
    template <class TClass, typename TField>
    TypeInfoBuilder &AddField(const std::string &fieldName, TField TClass::*fieldPointer)
    {
        mTypeInfo->mFields[fieldName] = std::make_unique<FieldInfo>(fieldName, fieldPointer, mTypeInfo.get());
        return *this;
    }
    template <class TClass, typename TRet, typename... TArgs>
    TypeInfoBuilder &AddMethod(const std::string &methodName, TRet (TClass::*methodPointer)(TArgs...))
    {
        mTypeInfo->mMethods[methodName] = std::make_unique<MethodInfo>(methodName, methodPointer, mTypeInfo.get());
        return *this;
    }
    template <class TClass, typename TRet, typename... TArgs>
    TypeInfoBuilder &AddMethod(const std::string &methodName, TRet (TClass::*methodPointer)(TArgs...) const)
    {
        mTypeInfo->mMethods[methodName] = std::make_unique<MethodInfo>(methodName, methodPointer, mTypeInfo.get());
        return *this;
    }
  private:
    std::unique_ptr<TypeInfo> mTypeInfo{nullptr};
};
} // namespace MReflection