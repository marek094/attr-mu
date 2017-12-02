//
//-- attr-MU lib
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#ifndef _ATTR_MU_ARRAY_WRAP_H_
#define _ATTR_MU_ARRAY_WRAP_H_


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

    template<ull... Vs>
    inline constexpr auto to_array() -> array_wrap<ull, sizeof...(Vs)> {
        return {Vs...};
    }
    
}} // namespace mu::impl

#endif // _ATTR_MU_ARRAY_WRAP_H_
