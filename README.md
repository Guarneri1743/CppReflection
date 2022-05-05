# CppReflection


CppReflection is a simple C++ reflection solution based on Clang. 

# Introduction

The purpose of this project is to help beginners like me to get a better picture of how the common C++ reflection system works. The most important part of this project is parsing AST and generating reflection code, source can be found at 'meta_gen.cpp'. 'reflection.hpp' is used to build the type descriptors, thus metadata can be accessed at runtime.

# Getting Started

- run 'setup.bat'
- open VisualStudio solution in build folder
- Annotate the fields, methods, class and struct that we are interested in

		#include "reflection.hpp"
		
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
			

- Build project 'MetaGen'

- Access metadata by GetType<T\>()
		
		#include "Foo.h"
		#ifdef _REFL_GEN_OFF_
		#include "Foo_gen_refl.h"
		#endif
		
		#include <iostream>
		
		using namespace Reflection;
		
		int main()
		{
			// use print helper
			auto& os = std::cout;
			GetType<Bar>()->Print(os, 0);
			
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
			
			// get/set field value
			Bar bar;
			bar.num = 13;

			void* obj = &bar;

			auto numField = GetType<Bar>()->GetField("num");
			auto numVal = numField->GetValue<int>(obj);
			std::cout << "num: " << numVal << std::endl;
			numField->SetValue(obj, 7);
			std::cout << "num: " << bar.num << std::endl;
			
			return 0;
		}

- Output:

		name: Bar
		size: 4
		type specifier type: kStruct
		ref declarator: kNone
		fields length:  1
		fields:
		  fields[0]:
		  name: Bar::num
		  type: int
		  offset: int
		  cvr qualifier: kNone
		  storage class specifier: kNone
		  thread storage class specifier: kUnSpecified
		  storage duration: kNone
		  access specifier: kPublic
		methods length:  0
		
		field: Foo::field1
		fieldType: float
		isConst: 1
		isVolatile: 1
		isPublic: 1
		isStatic: 0
		
		field: Foo::field2
		fieldType: Bar[10]
		isConst: 0
		isVolatile: 0
		isPublic: 1
		isStatic: 0
		
		field: Foo::field3
		fieldType: int
		isConst: 1
		isVolatile: 0
		isPublic: 1
		isStatic: 1
		
		field: Foo::field4
		fieldType: float
		isConst: 0
		isVolatile: 0
		isPublic: 1
		isStatic: 1

		num: 13
		num: 7

# Future works

- Automatic serialization code generation will be added as an example not long afterwards.


# References:

- [Building a C++ Reflection System Using LLVM and Clang](https://arvid.io/content/static/Reflection2.pdf)

- [metareflect](https://github.com/leandros/metareflect)

- [Getting Started: Building and Running Clang](https://clang.llvm.org/get_started.html)

- [LibASTMatchersTutorial](https://clang.llvm.org/docs/LibASTMatchersTutorial.html)





