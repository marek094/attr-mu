//
//-- attr-MU lib
//-- (c) 2017 Marek Cerny < me () marekcerny.com >
//

#include <cstdio>
#include <string>
#include <utility>

using size24 = long long[3];
//using size24 = std::string;

struct bad_S {
    char sign;
    size24 name;
    bool is_man;
    long double weight;
};

struct nice_S {
    size24 name;
    char sign;
bool is_man;
    long double weight;
};

struct packed_S {
    char sign;
    size24 name;
    bool is_man;
    long double weight;
} __attribute__((packed));

struct packed_aligned_S {
    bool is_man;
    char sign;
    size24 name;
    long double weight;
}__attribute__((packed, aligned));

using tuple_S = std::tuple<char, size24, bool, long double>;


#define PRINT(type) printf("| %-16s| %-15lu| %-15lu|\n", #type, sizeof(type), alignof(type))
#define PRINT_SEP   printf("+%s+\n", std::string(51, '-').c_str())
#define PRINT_HEAD  printf("| %-16s| %-15s| %-15s|\n", "type", "sizeof (bytes)", "alignof")

int _main() {
    PRINT_SEP;
    PRINT_HEAD;
    PRINT_SEP;
    PRINT(std::string);
    PRINT(size24);
    PRINT(bool);
    PRINT(char);
    PRINT(long double);
    PRINT_SEP;
    PRINT(bad_S);
    PRINT(nice_S);
    PRINT(packed_S);
    PRINT(packed_aligned_S);
    PRINT(tuple_S);
    PRINT_SEP;
    
    return 0;
}




