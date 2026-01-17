#include "MReflection.hpp"
#include <stdexcept>

namespace MReflection
{
Registry &Registry::GetInstance()
{
    static Registry instance;
    return instance;
}
TypeInfo *Registry::GetType(const std::string &name) const
{
    if (mTypes.contains(name))
    {
        return mTypes.at(name).get();
    }
    throw std::runtime_error("Type not found: " + name);
}
TypeInfoBuilder AddClass(std::string typeName)
{
    return TypeInfoBuilder(typeName);
}
} // namespace MReflection