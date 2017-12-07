//
//-- attr-MU lib - tests
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "./../attr_mu.hpp"

#include <vector>
#include <tuple>
#include <utility>
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
        
        wri2++;
        
        REQUIRE(j == 5);
        
    }
    
    SECTION( "mu::impl::dispatch" ) {
        
        REQUIRE( std::is_same<impl::dispatch<int>, impl::wrap<int> >::value == true );

        class C {};
        REQUIRE( std::is_same<impl::dispatch<C>, C>::value == true );
        
        REQUIRE( std::is_same<impl::dispatch<bool&>, impl::ref_wrap<bool>>::value == true );
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
    
    auto a = make_named_tuple( attr<"Hell"_mu, int>{3}
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
    
    SECTION( "Nested named_tuples" ) {
        
        auto x = make_named_tuple( make_attr<"integer"_mu>(123456)
                                  ,make_attr<"nested@1"_mu>(
                                      make_named_tuple( make_attr<"nested attr"_mu>(2.3f)
                                                       ,make_attr<"nested@2"_mu>(
                                                           make_named_tuple(
                                                               attr<"V$int"_mu, vector<int>>{1,2,3}
                                                           )
                                                        )
                                                       )
                                  )
        );
        
        x.at<"nested@1"_mu>().at<"nested@2"_mu>().at<"V$int"_mu>().push_back(999);
        
        REQUIRE( x.at<"nested@1"_mu>().at<"nested@2"_mu>().at<"V$int"_mu>().back() == 999 );
        
    }
    
    SECTION( "Multiple assignment" ) {
        
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
        
        REQUIRE( get<0>(z) == 42 );
        
        using tt_Ki = named_tuple<attr<"K"_mu, K>, attr<"i"_mu, int>>;
        using tt_iK = named_tuple<attr<"i"_mu, int>, attr<"K"_mu, K>>;
        
        K cc = F::get();
        attr<75, K> kk {std::move(cc)};

        tt_iK ik {6, F::get()};
        tt_Ki ki {attr<"K"_mu, K>{F::get()}, attr<"i"_mu, int>{9}};
        
//  TODO:
//        tt_Ki xx = move(ik);//{attr<"K"_mu, K>{F::get()}, attr<"i"_mu, int>{9}};
        
        
    }
    
    SECTION( "Partial constructor" ) {
        
        using nt_t = named_tuple<
            attr<"x"_mu, double>,
            attr<"y"_mu, double>,
            attr<"z"_mu, double>,
            attr<"a][a"_mu, vector<int>>,
            attr<"another"_mu, pair<int, int>>
        >;

        nt_t nt_piece {
              attr<"z"_mu, double>{1.},
              attr<"y"_mu, double>{.2},
              attr<"a][a"_mu, vector<int>>{1,2,3,4,5}
        };
        
        REQUIRE( nt_piece.at<"z"_mu>() == 1.0 );
        REQUIRE( nt_piece.at<"y"_mu>() == 0.2 );
        REQUIRE( nt_piece.at<"a][a"_mu>().back() == 5 );
        
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
        
        named_tuple<attr<"a"_mu,unsigned>, attr<"b"_mu, string>> z{v};
        
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

TEST_CASE( "A mu::impl::swuffle", "[mu::impl::swuffle]" ) {
    
    using st = named_tuple<attr<"s"_mu, string>, attr<"u"_mu, unsigned>, attr<"d"_mu, double>>;
    using nd = named_tuple<attr<"u"_mu, unsigned>, attr<"d"_mu, double>, attr<"s"_mu, string>>;
    
    st a {"a", 1u, .1};
    nd b {2u, .2, "b"};
    
    st x {b};
    
    REQUIRE( x.at<"s"_mu>() == "b" );
    
}

TEST_CASE( "Basic permute.hpp tests", "[permute]") {
    
    SECTION( "mu::sort") {
        for (int times = 4; times--; ) {
            array<int, 9> ar {{1,33,22,444,32,12,11,2,8}};
            std::random_shuffle(ar.begin(), ar.end());
            array<int, 9> ar2 = ar;
            mu::impl::sort(ar.begin(),   ar.end());
            std::sort(ar2.begin(), ar2.end());
            
//            for (auto p: ar) { cout << p << " "; } cout << endl;
//            for (auto p: ar2) { cout << p << " "; } cout << endl;
            
            REQUIRE( ar == ar2 );
        }
    }
    
    SECTION( "mu::sort - special cases") {
        array<int, 1> ar {{1}};
        mu::impl::sort(ar.begin(), ar.end());
        
        array<int, 2> ar2 {{2, 1}};
        mu::impl::sort(ar2.begin(), ar2.end());
        
        REQUIRE( ar2[0] < ar2[1] );
    }
    
    SECTION( "to_array" ) {
        auto v1 = impl::to_array<60, 50, 40, 30, 20, 10>();
        
        REQUIRE( v1[1] == 50u );
        REQUIRE( v1[3] == 30u );
    }
    
    SECTION( "to_sorted_array" ) {
        auto v1 = impl::to_sorted_array<60, 10, 40, 30, 20, 50>();
        auto v1_sorted = impl::to_array<10, 20, 30, 40, 50, 60>();
        
        REQUIRE( v1 == v1_sorted );
    }
    
    SECTION( "to_sizeof_array" ) {
        auto v1 = impl::to_sizeof_array<int, char, double>();
        auto v2 = impl::to_array<sizeof(int), sizeof(char), sizeof(double)>();
    
        REQUIRE( v1 == v2 );
    }
    
    SECTION( "to_alignof_array" ) {
        auto v1 = impl::to_alignof_array<int, char, double>();
        auto v2 = impl::to_array<alignof(int), alignof(char), alignof(double)>();
        
        REQUIRE( v1 == v2 );
    }
    
}
