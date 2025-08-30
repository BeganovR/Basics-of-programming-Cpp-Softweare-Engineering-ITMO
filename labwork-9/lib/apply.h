#pragma once
#include <type_traits>
#include <utility>

#include "tuple.h"

template <typename T>
struct is_member_function_pointer : std::is_member_function_pointer<T> {};

template <typename F, typename Tuple, std::size_t... I>
auto apply_impl(
    F&& f, Tuple&& t, std::index_sequence<I...>,
    std::enable_if_t<!is_member_function_pointer<std::decay_t<F>>::value>* =
        nullptr) {
    return std::forward<F>(f)(get<I>(std::forward<Tuple>(t))...);
}

template <typename F, typename Tuple, std::size_t... I>
auto apply_member_func_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
    return (get<0>(std::forward<Tuple>(t)).*f)(get<I+1>(std::forward<Tuple>(t))...);
}

template <typename F, typename Tuple, std::size_t... I>
auto apply_impl(
    F&& f, Tuple&& t, std::index_sequence<I...>,
    std::enable_if_t<is_member_function_pointer<std::decay_t<F>>::value>* = nullptr) {
    
    if constexpr (sizeof...(I) == 0) {
        return (get<0>(std::forward<Tuple>(t)).*f)();
    } else {
        return apply_member_func_impl(
            std::forward<F>(f), 
            std::forward<Tuple>(t), 
            std::make_index_sequence<sizeof...(I)>{}
        );
    }
}

template <typename F, typename... Args>
auto my_apply(F&& f, MyTuple<Args...>& t) {
    return apply_impl(std::forward<F>(f), t,
                      std::make_index_sequence<sizeof...(Args)>{});
}

template <typename F, typename... Args>
auto my_apply(F&& f, const MyTuple<Args...>& t) {
    return apply_impl(std::forward<F>(f), t,
                      std::make_index_sequence<sizeof...(Args)>{});
}

template <typename F, typename... Args>
auto my_apply(F&& f, MyTuple<Args...>&& t) {
    return apply_impl(std::forward<F>(f), std::move(t),
                      std::make_index_sequence<sizeof...(Args)>{});
}