//
//-- attr-MU lib
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#ifndef _ATTR_MU_BASE_H_
#define _ATTR_MU_BASE_H_

#include <utility>
#include <string>
#include <typeinfo>

namespace mu {
    
    namespace impl {
        using ull = unsigned long long;
    }
    
    template<typename T> struct type { using get = T; };
    template<typename T> using type_t = typename type<T>::get;
    
    struct list_nil {};
    
    template<typename... Ts> struct list {
        static constexpr type< list_nil > node{};
        static std::size_t size() { return 0; }
        static std::string to_string() { return ""; }
    };
    
    template<typename T, class... Ts>
    struct list<T, Ts...> : public list<Ts...> {
        static constexpr type< std::pair<T, list<Ts...>> > node{};
        static std::size_t size() { return sizeof...(Ts) + 1; }
        static std::string to_string() {
            return std::string{typeid(T).name()} + " " + list<Ts...>::to_string();
        }
        constexpr list() : list<Ts...>{}, val_{} { }
        template<typename Rest>
        constexpr list(T&& val, Rest&& vals)
            : list<Ts...>{std::forward<Rest>(vals)}
            , val_{std::forward<T>(val)} { }
        constexpr list(T&& val, Ts&&... vals)
            : list<Ts...>{std::forward<Ts>(vals)...}
            , val_{std::forward<T>(val)} { }
        T val_;
    };
    
    
    template<typename... Ts> struct cat {};
    
    template<typename... Ts, typename... TTs>
    struct cat<list<Ts...>, list<TTs...>> {
        using type = list<Ts...,TTs...>;
    };
    
    template <typename... All>
    using cat_t = typename cat<All...>::type;
    
    
    template<typename... Ts> struct shift {};
    
    template<typename T, typename... TTs>
    struct shift<T, list<TTs...>> {
        using type = typename cat<list<T>, list<TTs...>>::type;
    };
    
    template <typename... All>
    using shift_t = typename shift<All...>::type;
    
    template<typename RT, typename List>
    struct remove {
        using type = List;
    };
    
    template<typename RT, typename T, typename... Ts>
    struct remove<RT, list<T, Ts...>> {
        using type = typename std::conditional<
            std::is_same<RT, T>::value,
                typename remove<RT, list<Ts...>>::type,
                shift_t<T, typename remove<RT, list<Ts...>>::type>
        >::type;
    };
    
    template <typename... All>
    using remove_t = typename remove<All...>::type;
    
    
    
    
    template<typename List> struct bubble { using type = List; };
    
    template<typename T, typename TT, typename... Ts>
    struct bubble<list<T, TT, Ts...>> {
        using type = typename std::conditional<
            (alignof(T) > alignof(TT)),
                shift_t<T, typename bubble<list<TT,Ts...>>::type>,
                shift_t<TT,typename bubble<list<T, Ts...>>::type>
        >::type;
    };
    
    template<int I, typename Ignore> struct bubble_sort_impl {};
    template<int I, typename... Ts>
    struct bubble_sort_impl<I, list<Ts...>> {
        using type = typename bubble_sort_impl<I-1,
            typename bubble<
                list<Ts...>
            >::type
        >::type;
    };
    template<typename... Ts>
    struct bubble_sort_impl<0, list<Ts...>> {
        using type = typename bubble<list<Ts...>>::type;
    };
    
    
    template<typename List> struct bubble_sort { using type = List; };
    template<typename... Ts>
    struct bubble_sort<list<Ts...>> {
        using type = typename bubble_sort_impl<sizeof...(Ts), list<Ts...>>::type;
    };
    template<typename...All>
    using bubble_sort_t = typename bubble_sort<All...>::type;
    
    
    template<int I, typename List> struct drop { using type = list<>; };
    
    template<int I, typename T, typename... Ts>
    struct drop<I, list<T, Ts...>> {
        using type = typename std::conditional<
            (I > 0),
                typename drop<I-1, list<Ts...>>::type,
                list<T, Ts...>
            >::type;
    };
    
    template<int I, typename List>
    using drop_t = typename drop<I, List>::type;
    
    
    template<int I, typename List> struct take { using type = list<>; };
    
    template<int I, typename T, typename... Ts>
    struct take<I, list<T, Ts...>> {
        using type = typename std::conditional<
            (I > 0),
                shift_t<T, typename take<I-1, list<Ts...>>::type>,
                list<>
        >::type;
    };
    
    template<int I, typename List>
    using take_t = typename take<I, List>::type;
    
    
    template<typename L1, typename Empty> struct merge { using type = L1; };
    template<typename TT, typename... TTs>
    struct merge<list<>, list<TT, TTs...>> {
        using type = list<TT, TTs...>;
    };
    template<typename T, typename... Ts, typename TT, typename... TTs>
    struct merge<list<T, Ts...>, list<TT, TTs...>> {
        using type = typename std::conditional<
            (alignof(T) > alignof(TT)),
                shift_t<T , typename merge<list<   Ts...>, list<TT, TTs...>>::type>,
                shift_t<TT, typename merge<list<T, Ts...>, list<TTs...>>::type>
        >::type;
    };
    
    template<typename L1, typename L2>
    using merge_t = typename merge<L1, L2>::type;
    
    template<typename L1> struct merge_sort { using type = L1; };
    template<typename T>  struct merge_sort<list<T>> { using type = list<T>; };
    template<typename T, typename... Ts>
    struct merge_sort<list<T, Ts...>> {
        using type = merge_t<
            typename merge_sort< take_t< ((sizeof...(Ts)+1) / 2), list<T,Ts...>> >::type,
            typename merge_sort< drop_t< ((sizeof...(Ts)+1) / 2), list<T,Ts...>> >::type
        >;
    };
    
    template<typename L1>
    using merge_sort_t = typename merge_sort<L1>::type;
    
    
    template<int I, typename T>
    struct index {
        static constexpr int i = I;
        using type = T;
    };
    
    
    //    template <int I, typename...Ts> indexed_list_impl {};
    template <int I, typename T,  typename... Ts>
    struct indexed_list_impl {
        using type = shift_t<index<I, T>, typename indexed_list_impl<I+1, Ts...>::type>;
    };
    template <int I, typename T>
    struct indexed_list_impl<I, T> {
        using type = list<index<I, T>>;
    };
    
    
    template <typename T,  typename... Ts>
    struct indexed_list {
        using type = typename indexed_list_impl<0, T, Ts...>::type;
    };
    template <typename... Ts>
    using indexed_list_t = typename indexed_list<Ts...>::type;
    
    template<std::size_t... Ts>
    using seq = std::integer_sequence<std::size_t, Ts...>;
    

}

#endif // _ATTR_MU_BASE_H_
