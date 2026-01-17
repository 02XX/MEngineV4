#pragma once
#include <string>
#include <vector>

namespace MReflection
{
class ConstructorInfo;
class FieldInfo;
class MethodInfo;
class IReflect
{
  public:
    virtual ~IReflect() = default;
    /**
     * @brief 获取构造函数
     *
     * @param name 构造函数名
     * @return ConstructorInfo* 构造函数信息指针，若不存在则返回 nullptr
     */
    virtual ConstructorInfo *GetConstructor(const std::string &name) const = 0;
    /**
     * @brief 获取所有构造函数
     *
     * @return std::vector<ConstructorInfo *> 构造函数信息指针列表
     */
    virtual std::vector<ConstructorInfo *> GetConstructors() const = 0;
    // Method and Field
    /**
     * @brief 获取当前类以及基类中定义的方法
     *
     * @param name 方法名
     * @return MethodInfo* 方法信息指针，若不存在则返回 nullptr
     */
    virtual MethodInfo *GetMethod(const std::string &name) const = 0;
    /**
     * @brief 获取当前类以及基类中定义的字段
     *
     * @param name 字段名
     * @return FieldInfo* 字段信息指针，若不存在则返回 nullptr
     */
    virtual FieldInfo *GetField(const std::string &name) const = 0;
    // Fields and Methods
    /**
     * @brief 获取当前类型以及基类中定义的所有方法
     *
     * @return std::vector<MethodInfo *>  方法信息指针列表
     */
    virtual std::vector<MethodInfo *> GetMethods() const = 0;
    /**
     * @brief 获取当前类型以及基类中定义的所有字段
     *
     * @return std::vector<FieldInfo *> 字段信息指针列表
     */
    virtual std::vector<FieldInfo *> GetFields() const = 0;
    /**
     * @brief 获取当前类型声明的字段
     *
     * @param name 字段名
     * @return FieldInfo* 字段信息指针，若不存在则返回 nullptr
     */
    virtual FieldInfo *GetDeclaredField(const std::string &name) const = 0;
    /**
     * @brief 获取当前类型声明的方法
     *
     * @param name 方法名
     * @return MethodInfo* 方法信息指针，若不存在则返回 nullptr
     */
    virtual MethodInfo *GetDeclaredMethod(const std::string &name) const = 0;
    /**
     * @brief 获取当前类型声明的所有字段
     *
     * @return std::vector<FieldInfo *> 字段信息指针列表
     */
    virtual std::vector<FieldInfo *> GetDeclaredFields() const = 0;
    /**
     * @brief 获取当前类型声明的所有方法
     *
     * @return std::vector<MethodInfo *> 方法信息指针列表
     */
    virtual std::vector<MethodInfo *> GetDeclaredMethods() const = 0;
};
} // namespace MReflection