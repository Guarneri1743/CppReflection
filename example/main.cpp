#include <iostream>
#include <string>

#include "../src/reflection.hpp"

using namespace std;
using namespace Reflection;

STRUCT(Bar) { FIELD() int num; };

CLASS(Foo)
{
public:
    FIELD()
        static const int field1;

    FIELD()
        const volatile float field2;

    FIELD()
        Bar field3[10];

    FIELD()
        static thread_local float field4;

    METHOD()
        inline int Add(int a, int const& b, int* c, int const&& d, int** e) const
    {
        return a + b;
    }

    METHOD()
        static void Test(){}
};

#ifdef _REFL_GEN_OFF_
#include "main_gen_refl.h"
#endif

#include <vector>

int main()
{
    auto& os = std::cout;
	GetType<Bar>()->Print(os, 0);
    os << "\n";
    GetType<Foo>()->Print(os, 0);
    os << "\n";
    GetType<Bar[10]>()->Print(os, 0);
    os << "\n";
    GetType<int*>()->Print(os, 0);
    os << "\n";
    GetType<int**>()->Print(os, 0);
    os << "\n";
    GetType<int&>()->Print(os, 0);
    os << "\n";
    GetType<int&&>()->Print(os, 0);
    os << "\n";
	return 0;
}
