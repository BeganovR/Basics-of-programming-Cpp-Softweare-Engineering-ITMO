#pragma once
template <typename... Types>
class MyTuple;

template <>
class MyTuple<> {};

template <typename First, typename... Rest>
class MyTuple<First, Rest...> {
   public:
    MyTuple(First first, Rest... rest) : first_(first), rest_(rest...) {}

    First& get_first() { return first_; }
    const First& get_first() const { return first_; }

    MyTuple<Rest...>& get_rest() { return rest_; }
    const MyTuple<Rest...>& get_rest() const { return rest_; }

   private:
    First first_;
    MyTuple<Rest...> rest_;
};

template <size_t I, typename Tuple>
struct tuple_element;

template <typename First, typename... Rest>
struct tuple_element<0, MyTuple<First, Rest...>> {
    static First& get(MyTuple<First, Rest...>& t) { return t.get_first(); }
    static const First& get(const MyTuple<First, Rest...>& t) {
        return t.get_first();
    }
};

template <size_t I, typename First, typename... Rest>
struct tuple_element<I, MyTuple<First, Rest...>> {
    static auto& get(MyTuple<First, Rest...>& t) {
        return tuple_element<I - 1, MyTuple<Rest...>>::get(t.get_rest());
    }
    static const auto& get(const MyTuple<First, Rest...>& t) {
        return tuple_element<I - 1, MyTuple<Rest...>>::get(t.get_rest());
    }
};

template <size_t I, typename... Types>
auto& get(MyTuple<Types...>& t) {
    return tuple_element<I, MyTuple<Types...>>::get(t);
}

template <size_t I, typename... Types>
const auto& get(const MyTuple<Types...>& t) {
    return tuple_element<I, MyTuple<Types...>>::get(t);
}

template <size_t I, typename... Types>
auto&& get(MyTuple<Types...>&& t) {
    return std::forward<decltype(get<I>(t))>(get<I>(t));
}

