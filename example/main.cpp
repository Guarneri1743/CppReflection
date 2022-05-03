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

template<typename T>
void PrintType()
{
	auto type = GetType<T>();
	std::cout << type->name << " size: " << type->size << " RefDeclarator: " << std::to_string((Byte)type->GetRefDeclarator()) << " is_array: " << type->is_array << " array_lenght: " << type->array_length << " is_pointer: " << type->is_pointer << " raw_type: " << (type->GetRawType() != nullptr ? type->GetRawType()->name : "null") << std::endl;
    
	if (type->GetFieldsLength() > 0)
	{
		std::cout << " Fields:" << std::endl;
	}

	for(int i = 0; i < type->GetFieldsLength(); ++i)
	{
		auto field = type->GetField(i);
		auto fieldType = field->GetType();
		if (field->GetType()->is_array)
		{
			std::cout << "  " << fieldType->name << " " << field->name << "[" << field->GetType()->array_length << "]" << "  offset: " << field->offset << std::endl;
		}
		else
		{
			std::cout << "  " << fieldType->name << " " << field->name << "  offset: " << field->offset << std::endl;
		}
	}

	if (type->GetMethodsLength() > 0)
	{
		std::cout << " Methods:" << std::endl;
	}

	for (int i = 0; i < type->GetMethodsLength(); ++i)
	{
		auto method = type->GetMethod(i);
		std::cout << "  " << method->GetReturnType()->name << " " << method->name << "(";
		for (int j = 0; j < method->GetParameterLength(); ++j)
		{
			auto parameter = method->GetParameter(j);
			std::cout << parameter->GetType()->name << " " << parameter->name;
			if (j < method->GetParameterLength() - 1)
			{
				std::cout << ", ";
			}
		}
		std::cout << ")" << std::endl;
	}
}

#include <vector>

int main()
{
	PrintType<Bar>();
    PrintType<Foo>();
	PrintType<Bar[10]>();
    PrintType<int*>();
    PrintType<int**>();
    PrintType<int&>();
    PrintType<int&&>();
	return 0;
}
