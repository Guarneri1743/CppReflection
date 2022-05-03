# CppReflection

 CppReflection is a simple C++ reflection implementation based on Clang.

# Features

- Accessing Type/Field/Method Metadata.

# How to generate metadata?

1. Build and run clang.
2. Annotate the Types/Fields/Methods that we are interested in.
2. Collect the metadata of the code we previously annotated by Clang's ASTMatcher.
3. Generate reflection code by metadata.

# Example of accessing metadata

- run 'setup.bat'
- build Project 'MetaGen'
- run Project 'Example'
	
    	
Foo.h

		#include "reflection.hpp"
		
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
		};
		

Foo.cpp
		
		#include "Foo.h"
		#ifdef _REFL_GEN_OFF_
		#include "Foo_gen_refl.h"
		#endif

		#include <iostream>

		using namespace Reflection;

		int main()
		{
			auto type = GetType<T>();
	
			std::cout << type->name << " size: " << type->size << std::endl;
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
	
			for (int i = 0; i < type->GetMethodsLength(); ++i)
			{
				auto method = type->GetMethod(i);
				std::cout << method->GetReturnType()->name << " " << method->name << "(";
				for (int j = 0; j < method->GetParameterLength(); ++j)
				{
					auto parameter = method->GetParameter(j);
					std::cout << parameter->GetType()->name << " " << parameter->name;
					if (j < method->GetParameterLength() - 1)
					{
						std::cout << ", ";
					}
				}
				std::cout << ");" << std::endl;
			}

			return 0;
		}

# References:

[Building a C++ Reflection System Using LLVM and Clang](https://arvid.io/content/static/Reflection2.pdf)

[metareflect](https://github.com/leandros/metareflect)

[Getting Started: Building and Running Clang](https://clang.llvm.org/get_started.html)

[LibASTMatchersTutorial](https://clang.llvm.org/docs/LibASTMatchersTutorial.html)





