
#include "TypeInfoBuilder.hpp"
#include "MReflection.hpp"
namespace MReflection
{
TypeInfoBuilder &TypeInfoBuilder::AddBaseClass(const std::string &baseTClassName)
{
    TypeInfo *baseTypeInfo = Registry::GetInstance().GetType(baseTClassName);
    mTypeInfo->mBaseType = baseTypeInfo;
    return *this;
}
TypeInfoBuilder &TypeInfoBuilder::AddBaseClass(TypeInfo *baseTypeInfo)
{
    mTypeInfo->mBaseType = baseTypeInfo;
    return *this;
}
TypeInfoBuilder::~TypeInfoBuilder()
{
    Registry::GetInstance().mTypes[mTypeInfo->GetName()] = std::move(mTypeInfo);
}
} // namespace MReflection