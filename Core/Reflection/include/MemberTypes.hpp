#pragma once
#include <cstdint>

namespace MReflection
{
enum class MemberTypes : uint32_t
{
    Constructor = 0x01,
    Event = 0x02,
    Field = 0x04,
    Method = 0x08,
    Property = 0x10,
    TypeInfo = 0x20,
    Custom = 0x40,
    NestedType = 0x80,
    All = 0xBF // 除了 Custom 以外的所有位组合
};

inline MemberTypes operator|(MemberTypes a, MemberTypes b)
{
    return static_cast<MemberTypes>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline MemberTypes operator&(MemberTypes a, MemberTypes b)
{
    return static_cast<MemberTypes>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}
} // namespace MReflection