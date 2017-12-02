//
//-- attr-MU lib
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#ifndef _NAMED_TUPLE_H_
#define _NAMED_TUPLE_H_

#include "base.hpp"
#include "string_param.hpp"

namespace mu {
    namespace impl {

        // <Wrappers> {
        // wrapper for PODs
        template<typename T> struct wrap {
            wrap() = default;
            wrap(T val) : val{val} { }
            wrap(const wrap<T>& w) : val{w.val} { }
            operator T&() { return val; }
            operator const T&() const { return val; }
            T val;
        };
        
        // wraper for reference
        template<typename T> struct ref_wrap {
            ref_wrap() = default;
            ref_wrap(T& ref) : ptr{std::addressof(ref)} { }
            ref_wrap(T&& ref) : ptr{std::addressof(ref)} { }
            ref_wrap(const ref_wrap<T>& w) : ptr{w.ptr} { }
            operator T&() { return *ptr; }
            operator const T&() const { return *ptr; }
            T* ptr;
        };
        // } </Wrappers>
        
        // <Dispatch> {
        template<typename T>
        using inheritable = typename std::conditional<
            std::is_class<T>::value,
                T,
                wrap<T>
        >::type;
        
        template<typename T>
        using dispatch = typename std::conditional<
            std::is_reference<T>::value,
                ref_wrap<std::remove_reference_t<T>>,
                inheritable<T>
        >::type;
        // } </Dispatch>
    } // namespace impl;
    
    
    // Note: encaptulation vs. inheritace (-> better constructors)
    template<impl::ull Data, typename T>
    struct attr : impl::dispatch<T> {
        using impl::dispatch<T>::dispatch;
        
        constexpr attr() = default;
        
        template<typename X>
        constexpr attr(X&& v) : impl::dispatch<T>(std::forward<X>(v)) { }
        
        static constexpr impl::ull str = Data;
        static constexpr bool is_attr = true;
        
        T& value() & { return static_cast<impl::dispatch<T> &>(*this); }
        const T& value() const & { return static_cast<const impl::dispatch<T> &>(*this); }
        T&& value() && { return static_cast<impl::dispatch<T> &&>(*this); }
        const T&& value() const && { return static_cast<const impl::dispatch<T> &&>(*this); }
    };
    
    namespace impl {

        // <ImplObjects> {
        template<ull, typename...>
        struct find { void operator()() {} };
        // Search
        template<ull S, ull I, typename T, typename... Ts>
        struct find<S, list<attr<I,T>, Ts...>> : find<S, list<Ts...> > {
            using list_ = list<attr<S,T>, Ts...>;
            // Not found
            using find<S, list<Ts...> >::operator();
            // Found
            auto&& operator ()(list_& list) { return list.val_.value(); }
            auto&& operator ()(const list_& list) { return list.val_.value(); }
            auto&& operator ()(list_&& list) { return list.val_.value(); }
            auto&& operator ()(const list_&& list) { return list.val_.value(); }
        };
        
        // OutdatedNote: std::move from casted
        template<typename, typename List>
        struct swuffle { inline list<> operator()(const List&) { return {}; } };
        
        template<typename List, ull I, typename T, typename... Ts>
        struct swuffle<list<attr<I,T>, Ts...>, List> : swuffle<list<Ts...>, List> {
            using list_ = list<attr<I,T>, Ts...>;
            using swuffle<list<Ts...>, List>::operator();
            inline list_ operator()(const List& from) {
                // using 'piecewise constructor'
                return { impl::find<I, List>{}(from),
                    swuffle<list<Ts...>, List>{}(from) };
            }
            inline list_ operator()(List&& from) {
                using std::move;
                // move(from) twice, once we want to move only searched I index
                return { move( impl::find<I, List>{}( move(from) ) ),
                    move( swuffle<list<Ts...>, List>{}( move(from) ) ) };
            }
        };
        // } </ImplObjects>
    }
    
    template<typename... As>
    class data {
    private:
        using self_ = data<As...>;
        using unordered_type_ = list<As...>;
        using type_ = merge_sort_t<list<As...>>;
        static constexpr std::size_t size_ = sizeof...(As);
        
    public:
        template<typename...> friend class data;
        
        template<class = std::enable_if_t<(size_ > 0)>>
        constexpr data(As&&... values) : data_tupl_{
            // Note: shuffle values to be sorted
            impl::swuffle<type_, unordered_type_>{}({std::forward<As>(values)...})
        } { }

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
        inline void assign_impl(const list<>&) { }

        template<impl::ull I, typename T, typename... Ts>
        inline void assign_impl(const list<attr<I, T>, Ts...>& from) {
            impl::find<I, type_>{}(data_tupl_) = from.val_;
            assign_impl(static_cast<list<Ts...>>(from));
        }

        inline void assign_impl(list<>&&) {}
        
        template<impl::ull I, typename T, typename... Ts>
        inline void assign_impl(list<attr<I, T>, Ts...>&& from) {
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


#endif /* _NAMED_TUPLE_H_ */
