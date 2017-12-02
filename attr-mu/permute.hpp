//
//-- attr-MU lib
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#ifndef _PERMUTE_H_
#define _PERMUTE_H_

#include "base.hpp"
#include "array_wrap.hpp"

#include <array>
#include <algorithm>

namespace mu { namespace impl {

    template<class Iter>
    constexpr void sort(Iter begin, Iter end) {
        // Source: https://en.wikipedia.org/wiki/Quicksort
        if (begin >= end) return;
        
        // Note: *(begin + (end-begin)/2) is not constexpr in GCC
        auto pivot = *(begin);
        Iter left = begin;
        Iter right = end;
        do {
            while (*left < pivot && left < end-1) ++left;
            while (pivot < *(right-1) && begin < right-1) --right;
            if (left < right) {
                auto pom = *left;
                *left = *(right-1);
                *(right-1) = pom;
                ++left;
                --right;
            }
        } while (begin < right && left < right-1 );
        
        sort(begin, right);
        sort(left, end);
    }

    template<ull... Vs, class Func>
    constexpr auto map(Func&& f) -> array_wrap<ull, sizeof...(Vs)> {
        constexpr auto arr = to_array<Vs...>();
        return {(Vs, f(arr))...};
    }

    template<std::size_t N>
    constexpr ull s2(array_wrap<ull, N> arr) {
        return arr[0] << 2;
    }

    template<std::size_t N>
    constexpr array_wrap<ull, N> func(array_wrap<ull, N> arr) {
        array_wrap<ull, N> c = arr;
        mu::impl::sort(c.begin(), c.end());
        return c;
    }

    template<std::size_t N>
    constexpr array_wrap<ull, N> reorder_alg(array_wrap<ull, N> arr) {
        return {};
    }
    
    template<ull... Vs, std::size_t... Is, std::size_t N = sizeof...(Vs)>
    constexpr auto to_sorted_array_helper(std::index_sequence<Is...>) {
        constexpr auto arr = func(to_array<Vs...>());
        return to_array<arr[Is]...>();
    }

    template<ull... Vs>
    constexpr auto to_sorted_array() {
        return to_sorted_array_helper<Vs...>(std::make_index_sequence<sizeof...(Vs)>{});
    }

    template<unsigned I, ull... Vs>
    constexpr unsigned select() {
        auto arr = to_array<Vs...>();
        mu::impl::sort(arr.begin(), arr.end());
        return arr[I];
    }

    template<ull V>
    struct checks {
//        using type = checks< val<1, 10,20>() >;
    };


}} // namespace mu::impl
    
#endif /* _PERMUTE_H_ */
