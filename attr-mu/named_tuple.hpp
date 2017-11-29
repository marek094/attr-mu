//
//-- attr-MU lib
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#ifndef _NAMED_TUPLE_H_
#define _NAMED_TUPLE_H_

#include "base.hpp"

namespace mu {
    
    namespace impl {
        using ull = unsigned long long;
        
        template<typename T> struct wrap {
            wrap() = default;
            wrap(T val) : val{val} { }
            operator T&() & { return val; }
            operator const T&() const& { return val; }
            operator T&&() && { return val; }
            operator const T&&() const&& { return val; }
            T val;
        };
        
        template<typename T>
        using inheritable = typename std::conditional<std::is_class<T>::value, T, wrap<T>>::type;
    } // namespace impl;
    
    template<impl::ull Data, typename T>
    struct attr : impl::inheritable<T> {
        using impl::inheritable<T>::inheritable;
        constexpr attr() = default;
        static constexpr impl::ull str = Data;
        
        T& value() & { return static_cast<impl::inheritable<T> &>(*this); }
        const T& value() const & { return static_cast<const impl::inheritable<T> &>(*this); }
        T&& value() && { return static_cast<impl::inheritable<T> &&>(*this); }
        const T&& value() const && { return static_cast<const impl::inheritable<T> &&>(*this); }
    };
    
    namespace impl {
        constexpr inline impl::ull str_(const char *s) {
            impl::ull res = 0;
            for (; *s; ++s) res = res*128 + *s % 128;
            return res;
        }
        
        template<impl::ull, typename...>
        struct find { void operator()() {} };
        // Search
        template<impl::ull S, impl::ull I, impl::ull... Is, typename T, typename... Ts>
        struct find<S, mu::list<attr<I,T>, attr<Is,Ts>...>>
        : find<S, mu::list<attr<Is,Ts>...> > {
            using list_ = mu::list<attr<S,T>, attr<Is,Ts>...>;
            // Not found
            using find<S, mu::list<attr<Is,Ts>...> >::operator();
            // Found
            auto&& operator ()(list_& list) { return list.val_.value(); }
            auto&& operator ()(const list_& list) { return list.val_.value(); }
            auto&& operator ()(list_&& list) { return list.val_.value(); }
            auto&& operator ()(const list_&& list) { return list.val_.value(); }
        };
    }
    
    template<typename... As>
    class data {
    private:
        using self_ = data<As...>;
        using type_ = mu::list<As...>;
        
    public:
        constexpr data(As&&... values) : data_tupl_{std::forward<As>(values)...} { }
        template<class = std::enable_if_t<(sizeof...(As) > 0)>>
        constexpr data() {}
        
        template<impl::ull S>
        constexpr inline auto&& at() & {
            return impl::find<S, type_>{}(data_tupl_);
        }
        template<impl::ull S>
        constexpr inline auto&& at() const & {
            return impl::find<S, type_>{}(data_tupl_);
        }
        template<impl::ull S>
        constexpr inline auto&& at() && {
            return impl::find<S, type_>{}(data_tupl_);
        }
        template<impl::ull S>
        constexpr inline auto&& at() const && {
            return impl::find<S, type_>{}(data_tupl_);
        }
        
        template<impl::ull S>
        friend constexpr inline auto&& get(self_& data) noexcept {
            return impl::find<S, type_>{}(data.data_tupl_);
        }
        template<impl::ull S>
        friend constexpr inline auto&& get(const self_& data) noexcept {
            return impl::find<S, type_>{}(data.data_tupl_);
        }
        template<impl::ull S>
        friend constexpr inline auto&& get(self_&& data) noexcept {
            return impl::find<S, type_>{}(std::move(data.data_tupl_));
        }
        template<impl::ull S>
        friend constexpr inline auto&& get(const self_&& data) noexcept {
            return impl::find<S, type_>{}(std::move(data.data_tupl_));
        }
    private:
        type_ data_tupl_;
    };
    
    template<impl::ull... Is, typename... Ts>
    constexpr auto attr_mu(attr<Is,Ts>&&... args) {
        return data<attr<Is,Ts>...>(std::forward<attr<Is,Ts>>(args)...);
    }
    
    template<typename ...As>
    using named_tuple = data<As...>;

} // namespace mu;


constexpr auto operator"" _mu(const char* str, std::size_t) {
    return mu::impl::str_(str);
}

#endif /* _NAMED_TUPLE_H_ */
