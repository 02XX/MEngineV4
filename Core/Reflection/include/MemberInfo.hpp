#pragma once
#include "MemberTypes.hpp"
#include "NonCopyable.hpp"
#include <string>

namespace MReflection
{
class TypeInfo;
class MemberInfo : public NonCopyable
{
    friend class TypeInfoBuilder;

  protected:
    MemberInfo(const std::string &name, TypeInfo *declaringType) : mName(name), mDeclaringType(declaringType)
    {
    }

  public:
    virtual ~MemberInfo() = default;

    inline const std::string &GetName() const
    {
        return mName;
    }
    inline TypeInfo *GetDeclaringType() const
    {
        return mDeclaringType;
    };
    virtual MemberTypes GetMemberType() const = 0;
    bool operator==(const MemberInfo &other) const
    {
        return mName == other.mName && mDeclaringType == other.mDeclaringType &&
               GetMemberType() == other.GetMemberType();
    }

    bool operator!=(const MemberInfo &other) const
    {
        return !(*this == other);
    }

  private:
    std::string mName{"Unknown"};
    TypeInfo *mDeclaringType{nullptr};
};
} // namespace MReflection