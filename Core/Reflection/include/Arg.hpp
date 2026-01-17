#pragma once
#include <any>
#include <functional>
#include <stdexcept>
#include <tuple>

namespace MReflection
{
/**
 * @brief 参数封装类，将传入的任意形式参数（左右值，引用，常量...）Cast为目标类型
 *
 */
class Arg
{
  private:
    std::any mValue;
    bool mIsLValueRef{false};
    bool mIsConst{false};

  public:
    template <typename T> Arg(T &&value) // value最后只能是左值引用或者是右值引用
    {
        mIsLValueRef = std::is_lvalue_reference_v<T>;
        mIsConst = std::is_const_v<std::remove_reference_t<T>>;
        // 构造函数不允许显示指定模板参数
        // 因此在构造函数万能引用中，T&& value只有四种可能状态：
        // R& value, T is R&
        // const R& value, T is const R&
        // R&& value, T is R
        // const R&& value, T is const R
        if constexpr (std::is_lvalue_reference_v<T>) // T是左值引用
        {
            // value是左值引用
            // any会擦除传入参数的引用类型，如果不用std::ref包装，则会进行一次拷贝
            mValue = std::ref(value); // std::reference_wrapper<R> or std::reference_wrapper<const R>
        }
        else // T是值类型
        {
            // value是右值引用
            // any会擦除传入参数的引用类型，因此使用std::move进行一次移动语义。
            mValue = std::move(value); // R or const R
        }
    }
    template <typename TTarget> TTarget Cast()
    {
        using CleanTarget = std::remove_cv_t<std::remove_reference_t<TTarget>>;

        constexpr bool TargetIsRef = std::is_reference_v<TTarget>;
        constexpr bool TargetIsLValueRef = std::is_lvalue_reference_v<TTarget>;
        constexpr bool TargetIsRValueRef = std::is_rvalue_reference_v<TTarget>;
        constexpr bool TargetIsConst = std::is_const_v<std::remove_reference_t<TTarget>>;
        // T&& value四种状态 (R& value, const R& value, R&& value, const R&& value)
        // Target六种状态 (R, const R, R&, const R&, R&&, const R&&)
        // 4 x 6 = 24种转换可能性
        if (mIsLValueRef) // mValue是左值引用
        {
            if (mIsConst)
            {
                // 存储的是 std::reference_wrapper<const CleanTarget>
                auto ref = std::any_cast<std::reference_wrapper<const CleanTarget>>(mValue);
                if constexpr (TargetIsLValueRef)
                {
                    if constexpr (TargetIsConst)
                    {
                        return ref.get();
                    }
                    else
                    {
                        return const_cast<CleanTarget &>(ref.get());
                    }
                }
                else if constexpr (TargetIsRValueRef)
                {
                    if constexpr (TargetIsConst)
                    {
                        return std::move(ref.get());
                    }
                    else
                    {
                        return std::move(const_cast<CleanTarget &>(ref.get()));
                    }
                }
                else
                {
                    return ref.get();
                }
            }
            else
            {
                // 存储的是 std::reference_wrapper<CleanTarget>
                auto ref = std::any_cast<std::reference_wrapper<CleanTarget>>(mValue);
                if constexpr (TargetIsRValueRef)
                    return std::move(ref.get());
                else
                    return ref.get();
            }
        }
        else // mValue是右值引用
        {
            if (mIsConst)
            {
                // 存储的是 const CleanTarget
                auto &val = std::any_cast<const CleanTarget &>(mValue);
                if constexpr (TargetIsLValueRef)
                {
                    if constexpr (TargetIsConst)
                    {
                        return val;
                    }
                    else
                    {
                        return const_cast<CleanTarget &>(val);
                    }
                }
                else if constexpr (TargetIsRValueRef)
                {
                    if constexpr (TargetIsConst)
                    {
                        return std::move(val);
                    }
                    else
                    {
                        return std::move(const_cast<CleanTarget &>(val));
                    }
                }
                else
                {
                    return val;
                }
            }
            else
            {
                // 存储的是 CleanTarget
                auto &val = std::any_cast<CleanTarget &>(mValue);
                if constexpr (TargetIsRValueRef)
                    return std::move(val);
                else
                    return val;
            }
        }
    }
    template <typename... TArg, size_t... Is>
    static std::tuple<TArg...> ToTuple(std::array<Arg, sizeof...(TArg)> &argArray, std::index_sequence<Is...>)
    {
        return {argArray[Is].template Cast<TArg>()...};
    }
    template <typename... TArg> static std::tuple<TArg...> ToTuple(std::array<Arg, sizeof...(TArg)> &argArray)
    {
        return ToTuple<TArg...>(argArray, std::make_index_sequence<sizeof...(TArg)>{});
    }
};
} // namespace MReflection