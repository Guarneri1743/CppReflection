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
        const volatile float field1;

    FIELD()
        Bar field2[10];

    FIELD()
        static const int field3;

    FIELD()
        static thread_local float field4;


    METHOD()
        inline int Add(int a, int const& b, int* c, int const&& d, int** e) const
    {
        return a + b;
    }
};

#ifdef _REFL_GEN_OFF_
#include "main_gen_refl.h"
#endif

int main()
{
    // use print helper
    auto& os = std::cout;
	GetType<Bar>()->Print(os, 0);

    std::cout << std::endl;

    // access fields
    auto type = GetType<Foo>();
    for (int i = 0; i < type->GetFieldsLength(); ++i)
    {
        auto field = type->GetField(i);
        std::cout << "field: " << field->GetName() << std::endl;
        std::cout << "fieldType: " << field->GetType()->GetName() << std::endl;
        std::cout << "isConst: " << field->IsConst() << std::endl;
        std::cout << "isVolatile: " << field->IsVolatile() << std::endl;
        std::cout << "isPublic: " << field->IsPublic() << std::endl;
        std::cout << "isStatic: " << field->IsStatic() << std::endl;
        std::cout << std::endl;
    }

	Bar bar;
	bar.num = 13;

	// get/set field value
	void* obj = &bar;

	auto numField = GetType<Bar>()->GetField("num");
	auto numVal = numField->GetValue<int>(obj);
	std::cout << "num: " << numVal << std::endl;
	numField->SetValue(obj, 7);
	std::cout << "num: " << bar.num << std::endl;

	// TODO: invoke method

	return 0;
}
