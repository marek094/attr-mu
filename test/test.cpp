//
//-- attr-MU lib - tests
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "./../attr_mu.hpp"

#include <vector>
#include <tuple>
#include <string>
#include <type_traits>

using namespace std;
using namespace mu;


TEST_CASE( "A mu::named_tuple<> implementation", "[named_tuple_impl]") {
    
    SECTION ( " mu::impl::wrap<> ") {
        impl::wrap<int> wi = 5;
        
        REQUIRE( wi == 5);
        
        wi++;
        int i = wi;
        
        REQUIRE( wi == 6);
        REQUIRE( i == 6);
        
        int j = 4;
        attr<"x"_mu, int&> wri{j};
        attr<"x"_mu, int&> wri2 = std::move(wri);
        
    }
    
    SECTION( "mu::impl::inheritable" ) {
        
        class C {};
        REQUIRE( std::is_same<C, mu::impl::inheritable<C>>::value == true );
        REQUIRE( std::is_same<impl::wrap<int>, mu::impl::inheritable<int>>::value == true );
    }
    
    SECTION( "'\"\" _mu' literal" ) {
        
        impl::ull val = "123456789"_mu;
        std::string s = impl::rts_(val);
        
        REQUIRE( s == "123456789" );
        
        val = "1234567890"_mu;
        s = impl::rts_(val);
        
        REQUIRE( s == "234567890" );
    }
    
    SECTION( "Assigner" ) {
        
        using t_t = list<attr<"a"_mu, int>, attr<"b"_mu, string>>;
        using f_t = list<attr<"b"_mu, string>, attr<"a"_mu, int>>;
        t_t f = impl::swuffle<t_t,f_t>{}(f_t{"333",15});
        
    }
}

TEST_CASE( "A mu::named_tuple<> tests", "[named_tuple]" ) {
    
    auto a = attr_mu( attr<"Hell"_mu, int>{3}
                     ,attr<"Hi"_mu, double>(3.14)
                     ,attr<"Hi2"_mu, double>()
                     ,attr<"cool"_mu, vector<int>>{14,2,3,4}
                     ,attr<"boo"_mu, string>{"SS string"}
                     ,attr<"tuple"_mu, tuple<int, vector<int>>>{3,{55,666,7777}}
                     );
    
    SECTION( "Values of members by .at<...>()" ) {
    
        REQUIRE( a.at<"Hell"_mu>() == 3);
        REQUIRE( a.at<"cool"_mu>().front() == 14 );
        REQUIRE( a.at<"tuple"_mu>() ==  tuple<int, vector<int>>{3,{55,666,7777}});
        
    }
    
    SECTION( "Values of members by get<...>(...)" ) {
        
        REQUIRE( get<"Hell"_mu>(a) == 3);
        REQUIRE( get<"cool"_mu>(a).front() == 14 );
        REQUIRE( get<"tuple"_mu>(a) ==  tuple<int, vector<int>>{3,{55,666,7777}});
        REQUIRE( get<"boo"_mu>(a) == "SS string" );
        
    }
    
    SECTION( "Multiple assignment ") {
        
        REQUIRE( a.at<"Hi"_mu>() == 3.14 );

        a.at<"Hi"_mu>() = a.at<"Hi2"_mu>() = 9.999;
        
        REQUIRE( a.at<"Hi"_mu>() == 9.999 );
        REQUIRE( a.at<"Hi2"_mu>() == 9.999 );
        
    }
    
    SECTION( "Reference usage" ) {
        {
            get<1>(a.at<"tuple"_mu>()).push_back(44444);
        }
        REQUIRE(get<1>(get<"tuple"_mu>(a)).back() == 44444);
        
    }
    
    SECTION( "Move semantics partial" ) {
        
        REQUIRE( get<"boo"_mu>(a) == "SS string" );
        
        std::string boo = std::move(get<"boo"_mu>(a));
        
        REQUIRE( boo == "SS string" );
        REQUIRE( get<"boo"_mu>(a).empty() );
        
        a.at<"boo"_mu>() = "Second string";
        boo = std::move(a.at<"boo"_mu>());
        
        REQUIRE( boo == "Second string");
        REQUIRE( get<"boo"_mu>(a).empty() );
        
    }
    
    SECTION( "Move semantics global" ) {
        
        get<"boo"_mu>(a) = "New Content!";
        
        auto na = std::move(a);
        
        REQUIRE(na.at<"boo"_mu>() == "New Content!");
        REQUIRE(a.at<"boo"_mu>().empty() );
        
    }
    
    SECTION( "Move semantics const" ) {
        
        const named_tuple<attr<"s"_mu, string>> ct {"Long string"};
        named_tuple<attr<"ss"_mu, string>> ct2;
        
        REQUIRE( get<"s"_mu>(ct) == "Long string");
    
        ct2.at<"ss"_mu>() = std::move( ct.at<"s"_mu>() );
        
        REQUIRE( get<"ss"_mu>(ct2) == "Long string");
        REQUIRE( get<"s"_mu>(ct) == "Long string");
        
    }
    
    SECTION( "Forwarding references - tuple") {
        
        int i = 9;
        tuple<int&> t{i};
        get<0>(t)++;
        
        REQUIRE( i == 10 );
        
        tuple<int&> t2{t};
        get<0>(t2)++;
        
        REQUIRE( i == 11 );
        
        int j = 42;
        tuple<int&> t3{j};
        t3 = t2;
        
        REQUIRE( get<0>(t3) == 11 );
        
//        get<0>(t3)++;
//
//        REQUIRE( &i == &get<0>(t3) );
//        REQUIRE(  i ==  get<0>(t3) );
//        REQUIRE( i == 12 );
//        REQUIRE( &get<0>(t2) == &get<0>(t3) );
        
    }
        
    SECTION( "Forwarding references - named_tuple") {
        
        int i = 9;
        named_tuple<attr<""_mu, int&>> t{i};
        get<""_mu>(t)++;

        REQUIRE( i == 10 );

        named_tuple<attr<""_mu, int&>> t2{t};
        get<""_mu>(t2)++;

        REQUIRE( i == 11 );

        int j = 42;
        named_tuple<attr<""_mu, int&>> t3{j};

        REQUIRE( j == 42 );
        
    }

    SECTION( "Forwarding references - named_tuple cross_assign") {
        
        using st = named_tuple<attr<"r"_mu, int&>, attr<"u"_mu, unsigned>, attr<"d"_mu, double>>;
        using nd = named_tuple<attr<"u"_mu, unsigned>, attr<"d"_mu, double>, attr<"r"_mu, int&>>;

        int ai = 10;
        int bi = 20;
        st a {ai, 1u, .1};
        nd b {2u, .2, bi};

        get<"r"_mu>(a)++;
        get<"r"_mu>(b)++;

        REQUIRE( ai == 11 );
        REQUIRE( bi == 21 );
        REQUIRE( &ai == &get<"r"_mu>(a) );
        
        a = b;
// TODO:
//        REQUIRE( &get<"r"_mu>(a) == &get<"r"_mu>(b) );
//
//        get<"r"_mu>(a)++;
//        get<"r"_mu>(b)++;
//
//        REQUIRE( ai == 11 );
//        REQUIRE( bi == 23 );
    }
    
    SECTION( "Forwarding non-constructable" ) {
        class F;
        class K {
        private:
            friend class F;
            K() : x{3} { }
            K& operator=(const K&) { return *this; }
            K(const K& c) : x{3} { }

        public:
            K(K&& c) : x{3} { }
            K& operator=(K&&)  { return *this; }
            int x;
        };
        
        class F {
        public:
            static K get() {
                return {};
            }
        };
        
        tuple<int, K> x {42 , F::get()};
        
//        auto z = x; <-- not poss
        auto z = std::move(x);
        
        using tt_Ki = named_tuple<attr<"K"_mu, K>, attr<"i"_mu, int>>;
        using tt_iK = named_tuple<attr<"i"_mu, int>, attr<"K"_mu, K>>;
        
        K cc = F::get();
        attr<75, K> kk {std::move(cc)};

        tt_iK ik {6, F::get()};
        tt_Ki ki {attr<"K"_mu, K>{F::get()}, attr<"i"_mu, int>{9}};
        
//  TODO:
//        tt_Ki xx = move(ik);//{attr<"K"_mu, K>{F::get()}, attr<"i"_mu, int>{9}};
        
        
    }
    
}

TEST_CASE("A mu::named_tuple<> constructor swaps") {
    
    // Point:
    // tuple<attr<attr<"b"_mu, string>, "a"_mu,unsigned> v {};
    // tuple<attr<"a"_mu,unsigned>, attr<"b"_mu, string> x = v
    SECTION ( "Assing shufled and asymnetric assign" ) {
    
        named_tuple<attr<"b"_mu, string>, attr<"a"_mu, unsigned>> v {"onetwo", 34};
        named_tuple<attr<"a"_mu,unsigned>, attr<"b"_mu, string>> w {12, "threefour"};
        
        v = w;
        
        named_tuple<attr<"a"_mu,unsigned>, attr<"b"_mu, string>> z = v;
        
        REQUIRE(v.at<"a"_mu>() == 12);
        REQUIRE(v.at<"b"_mu>() == "threefour");
        
        REQUIRE(z.at<"a"_mu>() == 12);
        REQUIRE(z.at<"b"_mu>() == "threefour");

        named_tuple<
        attr<"b"_mu, string>,
        attr<"a"_mu, unsigned>,
        attr<"default"_mu, string>
        > x = v;

        REQUIRE( x.at<"default"_mu>().empty() );
        
    }
    
    SECTION ( "Assing with unique_ptr, precise forwarding" ) {
        named_tuple<attr<"ptr"_mu, unique_ptr<int>>, attr<"i"_mu, int>> ntp {new int(3), 4};
        named_tuple<attr<"ptr"_mu, unique_ptr<int>>, attr<"i"_mu, int>> ntp2;
        
//        ntp2 = ntp; <= Non-move Error - OK
        ntp2 = move(ntp);
        
        REQUIRE( * ntp2.at<"ptr"_mu>() == 3 );
        
        named_tuple<attr<"i"_mu, int>, attr<"ptr"_mu, unique_ptr<int>>> swaped_ntp;
        swaped_ntp = move(ntp2);
        
        REQUIRE( * swaped_ntp.at<"ptr"_mu>() == 3 );
    }
    
}

TEST_CASE( "A mu::impl::assigner", "[mu::impl::assigner]" ) {
    
    using st = named_tuple<attr<"s"_mu, string>, attr<"u"_mu, unsigned>, attr<"d"_mu, double>>;
    using nd = named_tuple<attr<"u"_mu, unsigned>, attr<"d"_mu, double>, attr<"s"_mu, string>>;
    
    st a {"a", 1u, .1};
    nd b {2u, .2, "b"};
    
    st x {b};
    
    REQUIRE( x.at<"s"_mu>() == "b" );
    
    
}




