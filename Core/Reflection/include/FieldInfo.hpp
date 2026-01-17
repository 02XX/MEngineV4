#pragma once
#include "MemberInfo.hpp"
#include "MemberTypes.hpp"
#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace MReflection
{
class TypeInfo;
class FieldInfo final : public MemberInfo
{
  public:
    template <class TTClass, typename TField>
    FieldInfo(const std::string &name, TField TTClass::*fieldPointer, TypeInfo *declaringType)
        : MemberInfo(name, declaringType)
    {
        // https://en.cppreference.com/w/cpp/language/typeid.html
        //  If type or the type of expression is a class type or a reference to a class type, then that class
        //  type cannot be an incomplete type.
        //  1) Refers to a std::type_info object representing type. If type is a reference type, the result refers to a
        //  std::type_info object representing the cv-unqualified version of the referenced type.
        mGetter = [fieldPointer](std::any obj) -> std::any {
            auto &instance = std::any_cast<std::reference_wrapper<const TTClass>>(obj).get();
            return instance.*fieldPointer;
        };
        mSetter = [fieldPointer](std::any obj, std::any value) {
            if constexpr (std::is_const_v<typename std::remove_reference<TField>::type>)
            {
                throw std::runtime_error("Cannot set value to const field");
            }
            else
            {
                auto &instance = std::any_cast<std::reference_wrapper<TTClass>>(obj).get();
                instance.*fieldPointer = std::any_cast<typename std::remove_cvref<TField>::type>(std::move(value));
            }
        };
    }
    inline MemberTypes GetMemberType() const override
    {
        return MemberTypes::Field;
    }

  public:
    /**
     * @brief 获取字段值
     *
     * @tparam TRet 返回值类型
     * @tparam TTClass 类类型
     * @param obj 对象实例，只能传入左值引用
     * @return TRet
     */
    template <class TRet, class TTClass>
        requires(!std::is_pointer_v<TTClass>)
    TRet GetValue(const TTClass &obj) const
    {
        return std::any_cast<TRet>(mGetter(std::cref(obj)));
    }
    /**
     * @brief 设置字段值
     *
     * @tparam TValue
     * @tparam TTClass
     * @param obj 对象实例，只能传入左值引用
     * @param value 字段值，可以传入左值或右值或值类型，但是一定是可拷贝的
     */
    template <class TValue, class TTClass>
        requires(!std::is_pointer_v<TTClass>)
    void SetValue(TTClass &obj, TValue &&value) const
    {
        mSetter(std::ref(obj), std::forward<TValue>(value));
        // TODO: 根据value的左右值类型进行不同的处理，避免左值赋值给std::any时的一次拷贝开销
    }
    /**
     * @brief 获取字段值
     *
     * @tparam TRet 返回值类型
     * @tparam TTClass 类类型
     * @param obj 对象实例指针
     * @return TRet
     */
    template <class TRet, class TTClass> TRet GetValue(const TTClass *obj) const
    {
        if (obj == nullptr)
        {
            throw std::invalid_argument("obj is nullptr");
        }
        return GetValue<TRet>(*obj);
    }
    /**
     * @brief 设置字段值
     *
     * @tparam TValue
     * @tparam TTClass
     * @param obj 对象实例指针
     * @param value 字段值，可以传入左值或右值或值类型，但是一定是可拷贝的
     */
    template <class TValue, class TTClass> void SetValue(TTClass *obj, TValue &&value) const
    {
        if (obj == nullptr)
        {
            throw std::invalid_argument("obj is nullptr");
        }
        SetValue(*obj, std::forward<TValue>(value));
    }
    /**
     * @brief 获取字段值
     *
     * @tparam TRet 返回值类型
     * @tparam TTClass 类类型
     * @param obj 对象实例共享指针
     * @return TRet
     */
    template <class TRet, class TTClass> TRet GetValue(const std::shared_ptr<TTClass> obj) const
    {
        return GetValue<TRet>(obj.get());
    }
    /**
     * @brief 设置字段值
     *
     * @tparam TValue
     * @tparam TTClass
     * @param obj 对象实例共享指针
     * @param value 字段值，可以传入左值或右值或值类型，但是一定是可拷贝的
     */
    template <class TValue, class TTClass> void SetValue(std::shared_ptr<TTClass> obj, TValue &&value) const
    {
        SetValue(obj.get(), std::forward<TValue>(value));
    }

  private:
    std::function<std::any(std::any)> mGetter{nullptr};
    std::function<void(std::any, std::any)> mSetter{nullptr};
};
} // namespace MReflection