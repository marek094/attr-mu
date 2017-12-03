# attr-MU
Experimental meta MU containing both aligned and size optimized structure `mu::named_tuple<...>`.

#### Demo
```C++
using point = named_tuple<
    attr<"x"_mu, double>,
    attr<"y"_mu, double>,
    attr<"z"_mu, double>,
    attr<"n a m e"_mu, std::string>
>;

point p1 { 1.5, 3.5, 0.5, "/src/img/point.jpg"s };
point p2 { make_attr<"x"_mu>(2.5), make_attr<"y"_mu>(2.6), make_attr<"z"_mu>(0.0) };

cout << p1.at<"n a m e"_mu>() << endl; // `/src/img/point.jpg`
cout << get<"x"_mu>( p2 ) << endl;     // `2.5`
```

## Motivation

Consider example:

```C++
using size24 = long long[3];
//using size24 = std::string;

struct bad_S { char sign; size24 name; bool is_man; long double weight; };

struct nice_S { size24 name; char sign; bool is_man; long double weight; };

struct packed_S { char sign; size24 name; bool is_man; long double weight;
} __attribute__((packed));

struct packed_aligned_S { char sign; size24 name; bool is_man; long double weight;
}__attribute__((packed, aligned));

using tuple_S = std::tuple<char, size24, bool, long double>;
```

The main point is that the order of attributes in declaration of class affects size of the structure in C++,
so we want to introduce something like `attr_mu::packed_tuple<...>` with named access to its elemets.

#### Statistics (clang 9.0.0. / gcc 5.4.1):

```
+-----------------------------------------+
| type            | sizeof [B] | alignof  |
+-----------------------------------------+
| size24          | 24         | 8        |
| bool            | 1          | 1        |
| char            | 1          | 1        |
| long double     | 16         | 16       |
+-----------------------------------------+
| bad_S           | 64         | 16       |
| nice_S          | 48         | 16       |
| packed_S        | 42         | 1        |
| packed_aligned_S| 48         | 16       |
| tuple_S         | 64         | 16       |
+-----------------------------------------+
```
Moreover when we set  `using size24 = std::string` with **gcc** we obtain: `warning: ignoring packed attribute because of unpacked non-POD field 'std::string packed_aligned_S::name'`.

See: [suggestion `__attribute__ ((reorder))`](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=31176)
