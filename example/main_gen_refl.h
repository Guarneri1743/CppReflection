// auto-generated file.
#pragma once
#include "reflection.hpp"



namespace Reflection
{
 
	template<>
	Type const* GetTypeImpl(Tag<Bar>) noexcept
	{
		static TypeStorage<Bar, 1, 0> typeStorage;
		static Type field_0_Type = *GetType<int>();
		typeStorage.fields[0].name = "Bar::num";
		typeStorage.fields[0].offset = offsetof(Bar, Bar::num);
		typeStorage.fields[0].type = &field_0_Type;
		typeStorage.fields[0].cvr_qualifier = CVRQualifier::kNone;
		typeStorage.fields[0].storage_class_specifier = StorageClassSpecifier::kNone;
		typeStorage.fields[0].thread_storage_class_specifier = ThreadStorageClassSpecifier::kUnSpecified;
		typeStorage.fields[0].storage_duration = StorageDuration::kNone;
		typeStorage.fields[0].access_specifier = AccessSpecifier::kPublic;

		static Type type("Bar", sizeof(Bar), TypeSpecifierType::kStruct, RefDeclarator::kNone, typeStorage.fields, typeStorage.kFieldsNum, typeStorage.methods, typeStorage.kMethodsNum);
		return &type;
	};

	DECLARE_TYPE(Bar[10]);
	DECLARE_TYPE(int&);
	DECLARE_TYPE(int*);
	DECLARE_TYPE(int&&);
	DECLARE_TYPE(int**);
 
	template<>
	Type const* GetTypeImpl(Tag<Foo>) noexcept
	{
		static TypeStorage<Foo, 4, 2> typeStorage;
		static Type field_0_Type = *GetType<float>();
		typeStorage.fields[0].name = "Foo::field2";
		typeStorage.fields[0].offset = offsetof(Foo, Foo::field2);
		typeStorage.fields[0].type = &field_0_Type;
		typeStorage.fields[0].cvr_qualifier = CVRQualifier::kConst | CVRQualifier::kVolatile;
		typeStorage.fields[0].storage_class_specifier = StorageClassSpecifier::kNone;
		typeStorage.fields[0].thread_storage_class_specifier = ThreadStorageClassSpecifier::kUnSpecified;
		typeStorage.fields[0].storage_duration = StorageDuration::kNone;
		typeStorage.fields[0].access_specifier = AccessSpecifier::kPublic;

		static Type field_1_Type = *GetType<Bar[10]>();
		typeStorage.fields[1].name = "Foo::field3";
		typeStorage.fields[1].offset = offsetof(Foo, Foo::field3);
		typeStorage.fields[1].type = &field_1_Type;
		typeStorage.fields[1].cvr_qualifier = CVRQualifier::kNone;
		typeStorage.fields[1].storage_class_specifier = StorageClassSpecifier::kNone;
		typeStorage.fields[1].thread_storage_class_specifier = ThreadStorageClassSpecifier::kUnSpecified;
		typeStorage.fields[1].storage_duration = StorageDuration::kNone;
		typeStorage.fields[1].access_specifier = AccessSpecifier::kPublic;

		static Type field_2_Type = *GetType<int>();
		typeStorage.fields[2].name = "Foo::field1";
		typeStorage.fields[2].type = &field_2_Type;
		typeStorage.fields[2].cvr_qualifier = CVRQualifier::kConst;
		typeStorage.fields[2].storage_class_specifier = StorageClassSpecifier::kStatic;
		typeStorage.fields[2].thread_storage_class_specifier = ThreadStorageClassSpecifier::kUnSpecified;
		typeStorage.fields[2].storage_duration = StorageDuration::kStatic;
		typeStorage.fields[2].access_specifier = AccessSpecifier::kPublic;

		static Type field_3_Type = *GetType<float>();
		typeStorage.fields[3].name = "Foo::field4";
		typeStorage.fields[3].type = &field_3_Type;
		typeStorage.fields[3].cvr_qualifier = CVRQualifier::kNone;
		typeStorage.fields[3].storage_class_specifier = StorageClassSpecifier::kStatic;
		typeStorage.fields[3].thread_storage_class_specifier = ThreadStorageClassSpecifier::kCXX11ThreadLocal;
		typeStorage.fields[3].storage_duration = StorageDuration::kThread;
		typeStorage.fields[3].access_specifier = AccessSpecifier::kPublic;

		static Type method_0_ret_type = *GetType<int>();
		typeStorage.methods[0].name = "Foo::Add";
		typeStorage.methods[0].linkage = Linkage::kExternalLinkage;
		typeStorage.methods[0].return_type = &method_0_ret_type;
		static Parameter method_0_parameters[5];
		static Type method_0_param_0_type = *GetType <int>();
		method_0_parameters[0].name = "a";
		method_0_parameters[0].type = &method_0_param_0_type;
		method_0_parameters[0].cvr_qualifier = CVRQualifier::kNone;
		method_0_parameters[0].ref_declarator = RefDeclarator::kNone;
		static Type method_0_param_1_type = *GetType <int&>();
		method_0_parameters[1].name = "b";
		method_0_parameters[1].type = &method_0_param_1_type;
		method_0_parameters[1].cvr_qualifier = CVRQualifier::kNone;
		method_0_parameters[1].ref_declarator = RefDeclarator::kLValueReference;
		static Type method_0_param_2_type = *GetType <int*>();
		method_0_parameters[2].name = "c";
		method_0_parameters[2].type = &method_0_param_2_type;
		method_0_parameters[2].cvr_qualifier = CVRQualifier::kNone;
		method_0_parameters[2].ref_declarator = RefDeclarator::kNone;
		static Type method_0_param_3_type = *GetType <int&&>();
		method_0_parameters[3].name = "d";
		method_0_parameters[3].type = &method_0_param_3_type;
		method_0_parameters[3].cvr_qualifier = CVRQualifier::kNone;
		method_0_parameters[3].ref_declarator = RefDeclarator::kRValueReference;
		static Type method_0_param_4_type = *GetType <int**>();
		method_0_parameters[4].name = "e";
		method_0_parameters[4].type = &method_0_param_4_type;
		method_0_parameters[4].cvr_qualifier = CVRQualifier::kNone;
		method_0_parameters[4].ref_declarator = RefDeclarator::kNone;
		typeStorage.methods[0].parameters = &method_0_parameters[0];
		typeStorage.methods[0].parameters_length = 5;
		typeStorage.methods[0].access_specifier = AccessSpecifier::kPublic;

		static Type method_1_ret_type = *GetType<void>();
		typeStorage.methods[1].name = "Foo::Test";
		typeStorage.methods[1].linkage = Linkage::kExternalLinkage;
		typeStorage.methods[1].return_type = &method_1_ret_type;
		typeStorage.methods[1].parameters = nullptr;
		typeStorage.methods[1].parameters_length = 0;
		typeStorage.methods[1].access_specifier = AccessSpecifier::kPublic;

		static Type type("Foo", sizeof(Foo), TypeSpecifierType::kClass, RefDeclarator::kNone, typeStorage.fields, typeStorage.kFieldsNum, typeStorage.methods, typeStorage.kMethodsNum);
		return &type;
	};

}
