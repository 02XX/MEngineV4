#pragma once
#include "TypeBuilder.hpp"
#include "TypeInfo.hpp"
#include "TypeInfoBuilder.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace MReflection
{
class Registry final
{
    friend class TypeInfoBuilder;
    friend TypeInfoBuilder AddClass(std::string typeName);
    template <class TClass> friend TypeBuilder<TClass> AddClass();

  public:
    static Registry &GetInstance();
    TypeInfo *GetType(const std::string &name) const;
    template <class TClass> TypeInfo *GetType() const
    {
        return GetType(typeid(TClass).name());
    }

  private:
    std::unordered_map<std::string, std::unique_ptr<TypeInfo>> mTypes;
};
TypeInfoBuilder AddClass(std::string typeName);
template <class TClass> TypeBuilder<TClass> AddClass()
{
    return TypeBuilder<TClass>();
};
} // namespace MReflection