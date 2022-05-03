#pragma once
#include <memory>
#include <type_traits>

#ifndef _REFL_GEN_OFF_
#define CLASS(class_name, ...) class __attribute__((annotate("reflect" #__VA_ARGS__))) class_name
#define STRUCT(struct_name, ...) struct __attribute__((annotate("reflect" #__VA_ARGS__))) struct_name
#define FIELD(...) __attribute__((annotate("reflect" #__VA_ARGS__)))
#define METHOD(...) __attribute__((annotate("reflect" #__VA_ARGS__))) 
#else
#define CLASS(class_name, ...) class class_name
#define STRUCT(struct_name, ...) struct struct_name
#define FIELD(...) 
#define METHOD(...)
#endif

#define MEMCPY(src, dst, size) std::memcpy(src, dst, size)

namespace Reflection
{
	char kUnknownName[] = "Unknown";

	typedef size_t Offset;
	typedef size_t Size;
	typedef void* Pointer;
	typedef uint8_t Byte;
	typedef Byte* BytePointer;

	template<typename T, Size FieldsNum, Size MethodsNum>
	struct TypeStorage;
	struct Field;
	struct Type;
	struct Method;
	template<typename T>
	struct Tag {};

	enum class TypeSpecifierType : Byte
	{
		kBuiltin,
		kClass,
		kStruct,
		kUnion,
		kEnum
	};

	enum class CVRQualifier : Byte
	{
		kNone = 0x0,
		kConst = 0x1,
		kVolatile = 0x2,
		kRestrict = 0x4
	};

	enum class RefDeclarator : Byte
	{
		kNone,
		kLValueReference,
		kRValueReference
	};

	enum class StorageClassSpecifier : Byte
	{
		kNone,
		kExtern,
		kStatic,
		kPrivateExtern,
		kAuto,
		kRegister
	};

	enum class ThreadStorageClassSpecifier : Byte
	{
		kUnSpecified = 0x0,
		kGNUThread = 0x1,
		kCXX11ThreadLocal = 0x2,
		kC11ThreadLocal = 0x4
	};

	enum class StorageDuration : Byte
	{
		kNone,
		kFullExpression,
		kAuto,
		kThread,
		kStatic,
		kDynamic
	};

	enum class AccessSpecifier : Byte
	{
		kPublic,
		kProtected,
		kPrivate,
		kNone
	};

	enum class Linkage : Byte
	{
		kNoLinkage,
		kInternalLinkage,
		kUniqueExternalLinkage,
		kVisibleNoLinkage, // inline
		kModuleInternalLinkage,
		kModuleLinkage,
		kExternalLinkage
	};

	template<typename TEnumType>
	struct support_bitwise_enum : std::false_type {};

	template<typename TEnumType>
	typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
		operator&(TEnumType left, TEnumType right)
	{
		return static_cast<TEnumType>(
			static_cast<std::underlying_type_t<TEnumType>>(left) &
			static_cast<std::underlying_type_t<TEnumType>>(right));
	}

	template<typename TEnumType>
	typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
		operator|(TEnumType left, TEnumType right)
	{
		return static_cast<TEnumType>(
			static_cast<std::underlying_type_t<TEnumType>>(left) |
			static_cast<std::underlying_type_t<TEnumType>>(right));
	}

	template<typename TEnumType>
	typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
		operator^(TEnumType left, TEnumType right)
	{
		return static_cast<TEnumType>(
			static_cast<std::underlying_type_t<TEnumType>>(left) ^
			static_cast<std::underlying_type_t<TEnumType>>(right));
	}

	template<typename TEnumType>
	typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
		operator~(TEnumType value)
	{
		return static_cast<TEnumType>(
			~static_cast<std::underlying_type_t<TEnumType>>(value));
	}

	template<typename TEnumType>
	typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
		operator&=(TEnumType& left, TEnumType right)
	{
		left = left & right;
		return left;
	}

	template<typename TEnumType>
	typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
		operator|=(TEnumType& left, TEnumType right)
	{
		left = left | right;
		return left;
	}

	template<typename TEnumType>
	typename std::enable_if_t<support_bitwise_enum<TEnumType>::value, TEnumType>
		operator^=(TEnumType& left, TEnumType right)
	{
		left = left ^ right;
		return left;
	}

	template<>
	struct support_bitwise_enum<TypeSpecifierType> : std::true_type {};

	template<>
	struct support_bitwise_enum<CVRQualifier> : std::true_type {};

	template<>
	struct support_bitwise_enum<RefDeclarator> : std::true_type {};

	template<>
	struct support_bitwise_enum<StorageClassSpecifier> : std::true_type {};

	template<>
	struct support_bitwise_enum<ThreadStorageClassSpecifier> : std::true_type {};

	template<>
	struct support_bitwise_enum<StorageDuration> : std::true_type {};

	template<>
	struct support_bitwise_enum<AccessSpecifier> : std::true_type {};

	struct Base
	{
		char const* name;
		Base() : name(kUnknownName) {}
		Base(const char* _name) : name(_name) {}
	};

	struct Parameter : public Base
	{
		Type const* type;
		CVRQualifier cvr_qualifier;
		RefDeclarator ref_declarator;

		Type const* GetType() const noexcept
		{
			return type;
		}
	};

	struct Field : public Base
	{
		Type const* type;
		Offset offset; 
		CVRQualifier cvr_qualifier;
		StorageClassSpecifier storage_class_specifier;
		ThreadStorageClassSpecifier thread_storage_class_specifier;
		StorageDuration storage_duration;
		AccessSpecifier access_specifier;

		Type const* GetType() const noexcept
		{
			return type;
		}

		bool IsPublic() const noexcept
		{
			return access_specifier == AccessSpecifier::kPublic;
		}

		bool IsProtected() const noexcept
		{
			return access_specifier == AccessSpecifier::kProtected;
		}

		bool IsPrivate() const noexcept
		{
			return access_specifier == AccessSpecifier::kPrivate;
		}

		bool IsStatic() const noexcept
		{
			return storage_class_specifier == StorageClassSpecifier::kStatic;
		}

		template<typename T>
		T GetValue(Pointer ptr) const noexcept
		{
			static_assert(std::is_trivially_copyable<T>::value);
			T value;
			MEMCPY(&value, (BytePointer)ptr + offset, sizeof(T));
			return value;
		}

		template<typename T>
		void SetValue(Pointer ptr, T const& value) const noexcept
		{
			static_assert(std::is_trivially_copyable<T>::value);
			MEMCPY((BytePointer)ptr + offset, &value, sizeof(T));
		}

		template<typename T>
		T& GetRef(Pointer ptr) const noexcept
		{
			return *static_cast<T*>((BytePointer)ptr + offset);
		}

		template<typename T>
		T* GetPtr(Pointer ptr) const noexcept
		{
			return static_cast<T*>((BytePointer)ptr + offset);
		}
	};

	struct Method : public Base
	{
		Type const* return_type;
		Parameter const* parameters;
		Size parameters_length;
		AccessSpecifier access_specifier;
		Linkage linkage;

		Type const* GetReturnType() const noexcept
		{
			return return_type;
		}

		Parameter const* GetParameter(Offset index) const noexcept
		{
			return &parameters[index];
		}

		Size GetParameterLength() const noexcept
		{
			return parameters_length;
		}

		bool IsPublic() const noexcept
		{
			return access_specifier == AccessSpecifier::kPublic;
		}

		bool IsProtected() const noexcept
		{
			return access_specifier == AccessSpecifier::kProtected;
		}

		bool IsPrivate() const noexcept
		{
			return access_specifier == AccessSpecifier::kPrivate;
		}
	};

	template<typename T, Size FieldsNum, Size MethodsNum>
	struct TypeStorage
	{
		static constexpr Size kFieldsNum = FieldsNum;
		static constexpr Size kMethodsNum = MethodsNum;

		Field fields[FieldsNum + 1];
		Method methods[MethodsNum + 1];
	};

	struct Type : public Base
	{
		Size size;
		TypeSpecifierType type_specifier_type;
		RefDeclarator ref_declarator;
		Field* fields;
		Size fields_length;
		Method* methods;
		Size methods_length;
		bool is_array;
		Size array_length;
		bool is_pointer;
		Type const* raw_type;

		Type(char const* _name, size_t _size, TypeSpecifierType _type_specifier_type) : 
			Type(_name, _size, _type_specifier_type, RefDeclarator::kNone, nullptr, 0, nullptr, 0, false, 0, false, nullptr)
		{}

		Type(char const* _name, size_t _size, TypeSpecifierType _type_specifier_type, RefDeclarator _ref_declarator) :
			Type(_name, _size, _type_specifier_type, _ref_declarator, nullptr, 0, nullptr, 0, false, 0, false, nullptr)
		{}

		Type(char const* _name,
			 size_t _size, 
			 TypeSpecifierType _type_specifier_type,
			 RefDeclarator _ref_declarator, 
			 Field* _fields, Size _fields_length,
			 Method* _methods, Size _methods_length) :
			Type(_name, 
				 _size, 
				 _type_specifier_type, 
				 _ref_declarator, 
				 _fields, _fields_length, 
				 _methods, _methods_length, 
				 false, 0, false, nullptr)
		{}

		Type(char const* _name,
			 size_t _size,
			 TypeSpecifierType _type_specifier_type,
			 RefDeclarator _ref_declarator,
			 bool _is_array, Size _array_length, Type const* _raw_type) :
			Type(_name,
				 _size,
				 _type_specifier_type,
				 _ref_declarator,
				 nullptr, 0, nullptr, 0,
				 _is_array, _array_length, false, _raw_type)
		{}

		Type(char const* _name,
			 size_t _size,
			 TypeSpecifierType _type_specifier_type,
			 RefDeclarator _ref_declarator,
			 bool _is_pointer, Type const* _raw_type) :
			Type(_name,
				 _size,
				 _type_specifier_type,
				 _ref_declarator,
				 nullptr, 0, nullptr, 0,
				 false, 0, _is_pointer, _raw_type)
		{}

		Type(char const* _name,
			 size_t _size,
			 TypeSpecifierType _type_specifier_type,
			 RefDeclarator _ref_declarator, Type const* _raw_type) :
			Type(_name,
				 _size,
				 _type_specifier_type,
				 _ref_declarator,
				 nullptr, 0, nullptr, 0,
				 false, 0, false, _raw_type)
		{}

		Type(char const* _name,
			 size_t _size, 
			 TypeSpecifierType _type_specifier_type, 
			 RefDeclarator _ref_declarator, 
			 Field* _fields, Size _fields_length, 
			 Method* _methods, Size _methods_length, 
			 bool _is_array, Size _array_length, bool _is_pointer, Type const* _raw_type) :
			Base(_name),
			size(_size),
			type_specifier_type(_type_specifier_type),
			ref_declarator(_ref_declarator),
			fields(_fields),
			fields_length(_fields_length),
			methods(_methods),
			methods_length(_methods_length),
			is_array(_is_array),
			array_length(_array_length),
			is_pointer(_is_pointer),
			raw_type(_raw_type)
		{}

		Type const* GetRawType() const noexcept
		{
			return raw_type;
		}

		Field* const GetField(Offset index) const noexcept
		{
			return &fields[index];
		}

		Size GetFieldsLength() const noexcept
		{
			return fields_length;
		}

		Method* const GetMethod(Offset index) const noexcept
		{
			return &methods[index];
		}

		Size GetMethodsLength() const noexcept
		{
			return methods_length;
		}

		TypeSpecifierType GetTypeSpecifierType() const
		{
			return type_specifier_type;
		}

		RefDeclarator GetRefDeclarator() const
		{
			return ref_declarator;
		}
	};

	template<typename T>
	Type const* GetType() noexcept;

	template<typename T>
	Type const* GetTypeImpl(Tag<T>) noexcept;

	template<typename T>
	Type const* GetType() noexcept
	{
		return GetTypeImpl(Tag<T>());
	}

#define DECLARE_TYPE(T) \
	template<> \
	Type const* GetTypeImpl(Tag<T>) noexcept \
	{ \
		if (std::is_pointer<T>::value) \
		{ \
			static Type type(#T, sizeof(T), TypeSpecifierType::kBuiltin, RefDeclarator::kNone, true, GetType<std::remove_pointer<T>::type>()); \
			return &type; \
		} \
		else if (std::is_array<T>::value) \
		{ \
			static Type type(#T, sizeof(T), TypeSpecifierType::kBuiltin, RefDeclarator::kNone, true, std::extent<T>::value, GetType<std::remove_extent<T>::type>()); \
			return &type; \
		} \
		else if(std::is_reference<T>::value) \
		{ \
			static Type type(#T, sizeof(T), TypeSpecifierType::kBuiltin, std::is_lvalue_reference<T>::value ? RefDeclarator::kLValueReference : RefDeclarator::kRValueReference, GetType<std::remove_reference<T>::type>()); \
			return &type; \
		} \
		else \
		{ \
			static Type typeCache(#T, sizeof(T), TypeSpecifierType::kBuiltin); \
			return &typeCache; \
		} \
	}

#define DECLARE_TYPE_WITH_SIZE(type, type_size) \
	template<> \
	Type const* GetTypeImpl(Tag<type>) noexcept \
	{ \
		static Type typeCache(#type, type_size, TypeSpecifierType::kBuiltin); \
		return &typeCache; \
	}

	DECLARE_TYPE(bool);
	DECLARE_TYPE(char);
	DECLARE_TYPE(unsigned char);
	DECLARE_TYPE(short);
	DECLARE_TYPE(unsigned short);
	DECLARE_TYPE(int);
	DECLARE_TYPE(unsigned int);
	DECLARE_TYPE(long);
	DECLARE_TYPE(unsigned long);
	DECLARE_TYPE(long long);
	DECLARE_TYPE(unsigned long long);
	DECLARE_TYPE(float);
	DECLARE_TYPE(double);
	DECLARE_TYPE(long double);
	DECLARE_TYPE_WITH_SIZE(void, 0);
}