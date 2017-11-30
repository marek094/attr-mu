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
        
        constexpr std::size_t len_(mu::impl::ull x) {
            std::size_t len = 0;
            for (; x; x /= 128) len++;
            return len;
        }
        
        struct string_ {
            constexpr string_() = default;
            operator std::string() {
                return {data, data+9};
            }
            char data[9]{0,0,0,0,0,0,0,0,0};
        };
        
        constexpr string_ rts_(mu::impl::ull x) {
            string_ s;
            for (auto it=s.data+8; x; x /= 128)
                *it-- = x%128;
            return s;
        }

        // wrapper for PODs
        template<typename T> struct wrap {
            wrap() = default;
            wrap(T val) : val{val} { }
            operator T&() { return val; }
            operator const T&() const { return val; }
            T val;
        };
        
        template<typename T>
        using inheritable = typename std::conditional<std::is_class<T>::value, T, wrap<T>>::type;
    } // namespace impl;
    
    
    template<impl::ull Data, typename T>
    struct attr : impl::inheritable<T> {
        using impl::inheritable<T>::inheritable;
        
        constexpr attr() = default;
        template<typename X>
        constexpr attr(X&& v) : impl::inheritable<T>(std::forward<X>(v)) { }
        
        static constexpr impl::ull str = Data;
        static constexpr bool is_attr = true;
        
        T& value() & { return static_cast<impl::inheritable<T> &>(*this); }
        const T& value() const & { return static_cast<const impl::inheritable<T> &>(*this); }
        T&& value() && { return static_cast<impl::inheritable<T> &&>(*this); }
        const T&& value() const && { return static_cast<const impl::inheritable<T> &&>(*this); }
    };
    
    namespace impl {
        constexpr inline ull str_(const char *s) {
            ull res = 0;
            for (; *s; ++s) res = res*128 + *s % 128;
            return res;
        }
        
        template<ull, typename...>
        struct find { void operator()() {} };
        // Search
        template<ull S, ull I, typename T, typename... Ts>
        struct find<S, list<attr<I,T>, Ts...>>
        : find<S, list<Ts...> > {
            using list_ = list<attr<S,T>, Ts...>;
            // Not found
            using find<S, list<Ts...> >::operator();
            // Found
            auto&& operator ()(list_& list) { return list.val_.value(); }
            auto&& operator ()(const list_& list) { return list.val_.value(); }
            auto&& operator ()(list_&& list) { return list.val_.value(); }
            auto&& operator ()(const list_&& list) { return list.val_.value(); }
        };
        
//  TODO:
//        template<typename...> struct assigner {};
//        template<ull I, typename List, typename T, typename... As>
//        struct assigner<list<attr<I, T>, As...>, List> {
//            auto operator()(const List& from) {
//                list<attr<I, T>, As...> res {
//                    find<I, List>{}(from),
//                    assigner<list<As...>>{}(from)
//                };
//
////                impl::find<I, type_>{}(data_tupl_) = from.val_;
////                assign_impl(static_cast<const list<Ts...>>(from));
//                return res;
//            }
//        };

        
    }
    
    template<typename... As>
    class data {
    private:
        using self_ = data<As...>;
        using type_ = mu::list<As...>;
//        TODO:
//        using type_ = mu::merge_sort_t<mu::list<As...>>;
        static constexpr std::size_t size_ = sizeof...(As);
        
    public:
        template<class = std::enable_if_t<(size_ > 0)>>
        constexpr data(As&&... values) : data_tupl_{std::forward<As>(values)...} { }

        template<typename...>
        friend class data;
        
        constexpr data() {}
        
        template<typename... Ts>
        self_& operator= (const data<Ts...>& from) {
            assign_impl(from.data_tupl_);
            return *this;
        }
        
        template<typename... Ts>
        self_& operator= (data<Ts...>&& from) {
            assign_impl(std::move(from.data_tupl_));
            return *this;
        }
        
        template<typename... AAs>
        data(const data<AAs...>& p) { assign_impl(p.data_tupl_); }
        
        template<typename... AAs>
        data(data<AAs...>&& p) { assign_impl(std::move(p.data_tupl_)); }
        
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
        
        // <Assign_impl> {
        void assign_impl(const list<>&) { }

        template<impl::ull I, typename T, typename... Ts>
        void assign_impl(const list<attr<I, T>, Ts...>& from) {
            impl::find<I, type_>{}(data_tupl_) = from.val_;
            assign_impl(static_cast<const list<Ts...>>(from));
        }

        void assign_impl(list<>&&) {}
        
        template<impl::ull I, typename T, typename... Ts>
        void assign_impl(list<attr<I, T>, Ts...>&& from) {
            impl::find<I, type_>{}(data_tupl_) = std::move(from.val_);
            assign_impl(static_cast<list<Ts...>>(std::move(from)));
        }
        // } </Assign_impl>
     
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
