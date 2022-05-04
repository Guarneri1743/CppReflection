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
		typeStorage.fields[0] = Reflection::Field("Bar::num", &field_0_Type, offsetof(Bar, Bar::num), CVRQualifier::kNone, StorageClassSpecifier::kNone, ThreadStorageClassSpecifier::kUnSpecified, StorageDuration::kNone, AccessSpecifier::kPublic);
		static Type type("Bar", sizeof(Bar), TypeSpecifierType::kStruct, typeStorage.fields, typeStorage.kFieldsNum, typeStorage.methods, typeStorage.kMethodsNum);
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
		static TypeStorage<Foo, 4, 1> typeStorage;
		static Type field_0_Type = *GetType<float>();
		typeStorage.fields[0] = Reflection::Field("Foo::field1", &field_0_Type, offsetof(Foo, Foo::field1), CVRQualifier::kConst | CVRQualifier::kVolatile, StorageClassSpecifier::kNone, ThreadStorageClassSpecifier::kUnSpecified, StorageDuration::kNone, AccessSpecifier::kPublic);
		static Type field_1_Type = *GetType<Bar[10]>();
		typeStorage.fields[1] = Reflection::Field("Foo::field2", &field_1_Type, offsetof(Foo, Foo::field2), CVRQualifier::kNone, StorageClassSpecifier::kNone, ThreadStorageClassSpecifier::kUnSpecified, StorageDuration::kNone, AccessSpecifier::kPublic);
		static Type field_2_Type = *GetType<int>();
		typeStorage.fields[2] = Reflection::Field("Foo::field3", &field_2_Type, 0, CVRQualifier::kConst, StorageClassSpecifier::kStatic, ThreadStorageClassSpecifier::kUnSpecified, StorageDuration::kStatic, AccessSpecifier::kPublic);
		static Type field_3_Type = *GetType<float>();
		typeStorage.fields[3] = Reflection::Field("Foo::field4", &field_3_Type, 0, CVRQualifier::kNone, StorageClassSpecifier::kStatic, ThreadStorageClassSpecifier::kCXX11ThreadLocal, StorageDuration::kThread, AccessSpecifier::kPublic);
		static Type method_0_ret_type = *GetType<int>();
		static Parameter method_0_parameters[5];
		static Type method_0_param_0_type = *GetType <int>();
		method_0_parameters[0] = Reflection::Parameter("a", &method_0_param_0_type, CVRQualifier::kNone, RefDeclarator::kNone);
		static Type method_0_param_1_type = *GetType <int&>();
		method_0_parameters[1] = Reflection::Parameter("b", &method_0_param_1_type, CVRQualifier::kNone, RefDeclarator::kLValueReference);
		static Type method_0_param_2_type = *GetType <int*>();
		method_0_parameters[2] = Reflection::Parameter("c", &method_0_param_2_type, CVRQualifier::kNone, RefDeclarator::kNone);
		static Type method_0_param_3_type = *GetType <int&&>();
		method_0_parameters[3] = Reflection::Parameter("d", &method_0_param_3_type, CVRQualifier::kNone, RefDeclarator::kRValueReference);
		static Type method_0_param_4_type = *GetType <int**>();
		method_0_parameters[4] = Reflection::Parameter("e", &method_0_param_4_type, CVRQualifier::kNone, RefDeclarator::kNone);
		typeStorage.methods[0] = Reflection::Method("Foo::Add", &method_0_ret_type, &method_0_parameters[0], 5, AccessSpecifier::kPublic, Linkage::kExternalLinkage);
		static Type type("Foo", sizeof(Foo), TypeSpecifierType::kClass, typeStorage.fields, typeStorage.kFieldsNum, typeStorage.methods, typeStorage.kMethodsNum);
		return &type;
	};

}
