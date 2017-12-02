//
//-- attr-MU lib
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#ifndef _PERMUTE_H_
#define _PERMUTE_H_

#include "base.hpp"

#include <array>
#include <algorithm>

namespace mu { namespace impl {

    template<typename T, std::size_t N>
    struct array_wrap {
        constexpr array_wrap() = default;
        template<typename... Ts, class = std::enable_if_t<(sizeof...(Ts) == N)>>
        constexpr array_wrap(Ts... args) : data{args...} { }
        constexpr array_wrap(array_wrap&& aw) = default;
        constexpr array_wrap(const array_wrap& aw) = default;
        constexpr bool operator==(const array_wrap& p) const {
            for (std::size_t i=0; i < N; ++i) if (data[i] != p.data[i]) return false;
            return true;
        }
        constexpr bool operator!=(const array_wrap& p) const { return !(*this == p); }
        constexpr T* begin() { return &data[0]; }
        constexpr T* end() { return &data[0] + N; }
        constexpr T& operator[](std::size_t s) { return data[s]; }
        constexpr const T& operator[](std::size_t s) const { return data[s]; }
        using type = T;
        constexpr std::size_t size() { return N; }
        T data[N];
    };

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

    template<ull... Vs>
    constexpr auto to_array() -> array_wrap<ull, sizeof...(Vs)> {
        return {Vs...};
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
