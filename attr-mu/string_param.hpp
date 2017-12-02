//
//-- attr-MU lib
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#ifndef _ATTR_MU_STRING_PARAM_H_
#define _ATTR_MU_STRING_PARAM_H_

#include "array_wrap.hpp"

namespace mu {
    
    namespace impl {
        
        constexpr inline ull str_(const char *s) {
            ull res = 0;
            for (; *s; ++s) res = res*128 + *s % 128;
            return res;
        }
        
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

    } // namespace impl
    
} // namespace mu

// out of namespace
constexpr auto operator"" _mu(const char* str, std::size_t) {
    return mu::impl::str_(str);
}


#endif // _ATTR_MU_STRING_PARAM_H_
