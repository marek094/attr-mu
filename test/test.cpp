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
        mu::impl::wrap<int> wi = 5;
        
        REQUIRE( wi == 5);
        
        wi++;
        int i = wi;
        
        REQUIRE( wi == 6);
        REQUIRE( i == 6);
    
    }
    
    SECTION( "mu::impl::inheritable" ) {
        
        class C {};
        REQUIRE( std::is_same<C, mu::impl::inheritable<C>>::value == true );
        REQUIRE( std::is_same<impl::wrap<int>, mu::impl::inheritable<int>>::value == true );
    }
    
    SECTION( " '\"\" _mu' literal " ) {
        
        impl::ull val = "123456789"_mu;
        std::string s = impl::rts_(val);
        
        REQUIRE( s == "123456789" );
        
        val = "1234567890"_mu;
        s = impl::rts_(val);
        
        REQUIRE( s == "234567890" );
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
        
        const named_tuple<attr<"s"_mu, string>> ct {{"Long string"}};
        named_tuple<attr<"ss"_mu, string>> ct2;
        
        REQUIRE( get<"s"_mu>(ct) == "Long string");
    
        ct2.at<"ss"_mu>() = std::move( ct.at<"s"_mu>() );
        
        REQUIRE( get<"ss"_mu>(ct2) == "Long string");
        REQUIRE( get<"s"_mu>(ct) == "Long string");
        
    }

}
