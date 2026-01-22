#pragma once
#include "TypeInfoBuilder.hpp"

namespace MReflection
{
template <class TClass> class TypeBuilder : public TypeInfoBuilder
{
  public:
    TypeBuilder() : TypeInfoBuilder(typeid(TClass).name())
    {
    }

    template <class... TArgs> TypeBuilder &AddConstructor(const std::string &constructorName)
    {
        TypeInfoBuilder::AddConstructor<TClass, TArgs...>(constructorName);
        return *this;
    }
    template <class TBase> TypeBuilder &AddBaseClass()
    {
        TypeInfoBuilder::AddBaseClass(typeid(TBase).name());
        return *this;
    }
    TypeInfoBuilder &AddBaseClass(const std::string &baseTClassName) override
    {
        return TypeInfoBuilder::AddBaseClass(baseTClassName);
    }
    TypeInfoBuilder &AddBaseClass(TypeInfo *baseTypeInfo) override
    {
        return TypeInfoBuilder::AddBaseClass(baseTypeInfo);
    }
};
} // namespace MReflection