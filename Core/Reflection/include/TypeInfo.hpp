#pragma once
#include "ConstructorInfo.hpp"
#include "FieldInfo.hpp"
#include "IReflect.hpp"
#include "MemberInfo.hpp"
#include "MemberTypes.hpp"
#include "MethodInfo.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace MReflection
{
class TypeInfo final : public MemberInfo, public IReflect
{
    friend class TypeInfoBuilder;

  public:
    TypeInfo(const std::string &name) : MemberInfo(name, nullptr)
    {
    }
    TypeInfo *GetBaseType() const;
    inline MemberTypes GetMemberType() const override
    {
        return MemberTypes::TypeInfo;
    }
    ConstructorInfo *GetConstructor(const std::string &name) const override;
    std::vector<ConstructorInfo *> GetConstructors() const override;

    FieldInfo *GetField(const std::string &name) const override;
    MethodInfo *GetMethod(const std::string &name) const override;

    std::vector<MethodInfo *> GetMethods() const override;
    std::vector<FieldInfo *> GetFields() const override;

    FieldInfo *GetDeclaredField(const std::string &name) const override;
    MethodInfo *GetDeclaredMethod(const std::string &name) const override;

    std::vector<FieldInfo *> GetDeclaredFields() const override;
    std::vector<MethodInfo *> GetDeclaredMethods() const override;

  private:
    std::unordered_map<std::string, std::unique_ptr<FieldInfo>> mFields;
    std::unordered_map<std::string, std::unique_ptr<MethodInfo>> mMethods;
    std::unordered_map<std::string, std::unique_ptr<ConstructorInfo>> mConstructors;
    TypeInfo *mBaseType{nullptr};
};
} // namespace MReflection