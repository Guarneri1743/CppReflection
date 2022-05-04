#pragma once
#include <ostream>
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

#define REFL_MEMCPY(src, dst, size) std::memcpy(src, dst, size)

namespace Reflection
{
	constexpr char kDefaultName[] = "Unknown";

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
		kUnSpecified,
		kGNUThread,
		kCXX11ThreadLocal,
		kC11ThreadLocal
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
	struct support_bitwise_enum<CVRQualifier> : std::true_type {};

	std::ostream& operator<<(std::ostream& stream, TypeSpecifierType const& value)
	{
		switch (value)
		{
		case TypeSpecifierType::kBuiltin:
			stream << "kBuiltin";
			break;
		case TypeSpecifierType::kClass:
			stream << "kClass";
			break;
		case TypeSpecifierType::kEnum:
			stream << "kEnum";
			break;
		case TypeSpecifierType::kStruct:
			stream << "kStruct";
			break;
		case TypeSpecifierType::kUnion:
			stream << "kUnion";
			break;
		}
		return stream;
	}

	std::ostream& operator<<(std::ostream& stream, CVRQualifier const& value)
	{
		if (value == CVRQualifier::kNone)
		{
			stream << "kNone";
			return stream;
		}

		bool appendSeperator = false;

		if ((value & CVRQualifier::kConst) != CVRQualifier::kNone)
		{
			stream << "kConst";
			appendSeperator = true;
		}

		if ((value & CVRQualifier::kVolatile) != CVRQualifier::kNone)
		{
			if (appendSeperator)
			{
				stream << " | ";
			}

			stream << "kVolatile";
			appendSeperator = true;
		}

		if ((value & CVRQualifier::kRestrict) != CVRQualifier::kNone)
		{
			if (appendSeperator)
			{
				stream << " | ";
			}

			stream << "kRestrict";
		}

		return stream;
	}

	std::ostream& operator<<(std::ostream& stream, RefDeclarator const& value)
	{
		switch (value)
		{
		case RefDeclarator::kNone:
			stream << "kNone";
			break;
		case RefDeclarator::kLValueReference:
			stream << "kLValueReference";
			break;
		case RefDeclarator::kRValueReference:
			stream << "kRValueReference";
			break;
		}
		return stream;
	}

	std::ostream& operator<<(std::ostream& stream, StorageClassSpecifier const& value)
	{
		switch (value)
		{
		case StorageClassSpecifier::kNone:
			stream << "kNone";
			break;
		case StorageClassSpecifier::kExtern:
			stream << "kExtern";
			break;
		case StorageClassSpecifier::kStatic:
			stream << "kStatic";
			break;
		case StorageClassSpecifier::kPrivateExtern:
			stream << "kPrivateExtern";
			break;
		case StorageClassSpecifier::kAuto:
			stream << "kAuto";
			break;
		case StorageClassSpecifier::kRegister:
			stream << "kRegister";
			break;
		}
		return stream;
	}

	std::ostream& operator<<(std::ostream& stream, ThreadStorageClassSpecifier const& value)
	{
		switch (value)
		{
		case ThreadStorageClassSpecifier::kC11ThreadLocal:
			stream << "kC11ThreadLocal";
			break;
		case ThreadStorageClassSpecifier::kCXX11ThreadLocal:
			stream << "kCXX11ThreadLocal";
			break;
		case ThreadStorageClassSpecifier::kGNUThread:
			stream << "kGNUThread";
			break;
		case ThreadStorageClassSpecifier::kUnSpecified:
			stream << "kUnSpecified";
			break;
		}
		return stream;
	}
	
	std::ostream& operator<<(std::ostream& stream, StorageDuration const& value)
	{
		switch (value)
		{
		case StorageDuration::kNone:
			stream << "kNone";
			break;
		case StorageDuration::kFullExpression:
			stream << "kFullExpression";
			break;
		case StorageDuration::kAuto:
			stream << "kAuto";
			break;
		case StorageDuration::kThread:
			stream << "kThread";
			break;
		case StorageDuration::kStatic:
			stream << "kStatic";
			break;
		case StorageDuration::kDynamic:
			stream << "kDynamic";
			break;
		}
		return stream;
	}

	std::ostream& operator<<(std::ostream& stream, AccessSpecifier const& value)
	{
		switch (value)
		{
		case AccessSpecifier::kNone:
			stream << "kNone";
			break;
		case AccessSpecifier::kPrivate:
			stream << "kPrivate";
			break;
		case AccessSpecifier::kProtected:
			stream << "kProtected";
			break;
		case AccessSpecifier::kPublic:
			stream << "kPublic";
			break;
		}
		return stream;
	}
	
	std::ostream& operator<<(std::ostream& stream, Linkage const& value)
	{
		switch (value)
		{
		case Linkage::kNoLinkage:
			stream << "kNoLinkage";
			break;
		case Linkage::kInternalLinkage:
			stream << "kInternalLinkage";
			break;
		case Linkage::kUniqueExternalLinkage:
			stream << "kUniqueExternalLinkage";
			break;
		case Linkage::kVisibleNoLinkage:
			stream << "kVisibleNoLinkage";
			break;
		case Linkage::kModuleInternalLinkage:
			stream << "kModuleInternalLinkage";
			break;
		case Linkage::kModuleLinkage:
			stream << "kModuleLinkage";
			break;
		case Linkage::kExternalLinkage:
			stream << "kExternalLinkage";
			break;
		}
		return stream;
	}

	class Base
	{
	protected:
		char const* name;

	public:
		constexpr Base() : name(kDefaultName) {}
		constexpr Base(const char* _name) : name(_name) {}
		char const* GetName() const { return this->name; }
		virtual void Print(std::ostream& os, int indent) const;
	};

	class Parameter: public Base
	{
	private:
		Type const* type;
		CVRQualifier cvr_qualifier;
		RefDeclarator ref_declarator;

	public:
		constexpr Parameter() : Base(kDefaultName), type(nullptr), cvr_qualifier(CVRQualifier::kNone), ref_declarator(RefDeclarator::kNone) {}
		constexpr Parameter(
			const char* _name, 
			Type const* _type,
			CVRQualifier _cvr_qualifier,
			RefDeclarator _ref_declarator
		) : 
			Base(_name), 
			type(_type), 
			cvr_qualifier(_cvr_qualifier), 
			ref_declarator(_ref_declarator) 
		{}

		Type const* GetType() const noexcept { return type; }
		CVRQualifier GetCVRQualifier() const noexcept { return cvr_qualifier; }
		bool IsLValueReference() const noexcept { return ref_declarator == RefDeclarator::kLValueReference; }
		bool IsRValueReference() const noexcept { return ref_declarator == RefDeclarator::kRValueReference; }
		bool IsReference() const noexcept { return ref_declarator != RefDeclarator::kNone; }
		void Print(std::ostream& os, int indent) const;
	};

	class Field : public Base
	{
	private:
		Type const* type;
		Offset offset; 
		CVRQualifier cvr_qualifier;
		StorageClassSpecifier storage_class_specifier;
		ThreadStorageClassSpecifier thread_storage_class_specifier;
		StorageDuration storage_duration;
		AccessSpecifier access_specifier;

	public:
		constexpr Field() :
			Base(kDefaultName),
			type(nullptr),
			offset(0),
			cvr_qualifier(CVRQualifier::kNone),
			storage_class_specifier(StorageClassSpecifier::kNone),
			thread_storage_class_specifier(ThreadStorageClassSpecifier::kUnSpecified),
			storage_duration(StorageDuration::kNone),
			access_specifier(AccessSpecifier::kNone)
		{}

		constexpr Field(
			const char* _name,
			Type const* _type,
			Offset _offset,
			CVRQualifier _cvr_qualifier,
			StorageClassSpecifier _storage_class_specifier,
			ThreadStorageClassSpecifier _thread_storage_class_specifier,
			StorageDuration _storage_duration,
			AccessSpecifier _access_specifier
		) :
			Base(_name),
			type(_type),
			offset(_offset),
			cvr_qualifier(_cvr_qualifier),
			storage_class_specifier(_storage_class_specifier),
			thread_storage_class_specifier(_thread_storage_class_specifier),
			storage_duration(_storage_duration),
			access_specifier(_access_specifier)
		{}

		Type const* GetType() const noexcept { return type; }
		AccessSpecifier GetAccessSpecifier() const noexcept { return access_specifier; }
		CVRQualifier GetCVRQualifier() const noexcept { return cvr_qualifier; }
		StorageClassSpecifier GetStorageClassSpecifier() const noexcept { return storage_class_specifier; }
		ThreadStorageClassSpecifier GetTSCSpecifier() const noexcept { return thread_storage_class_specifier; }
		StorageDuration GetStorageDuration() const noexcept { return storage_duration; }
		bool IsPublic() const noexcept { return access_specifier == AccessSpecifier::kPublic; }
		bool IsProtected() const noexcept { return access_specifier == AccessSpecifier::kProtected; }
		bool IsPrivate() const noexcept { return access_specifier == AccessSpecifier::kPrivate; }
		bool IsStatic() const noexcept { return storage_class_specifier == StorageClassSpecifier::kStatic; }
		bool IsConst() const noexcept { return (cvr_qualifier & CVRQualifier::kConst) != CVRQualifier::kNone; }
		bool IsVolatile() const noexcept { return (cvr_qualifier & CVRQualifier::kVolatile) != CVRQualifier::kNone; }
		bool IsThreadLocal() const noexcept { return thread_storage_class_specifier != ThreadStorageClassSpecifier::kUnSpecified; }

		template<typename T>
		T GetValue(Pointer ptr) const noexcept
		{
			static_assert(std::is_trivially_copyable<T>::value);
			T value;
			REFL_MEMCPY(&value, (BytePointer)ptr + offset, sizeof(T));
			return value;
		}

		template<typename T>
		void SetValue(Pointer ptr, T const& value) const noexcept
		{
			static_assert(std::is_trivially_copyable<T>::value);
			REFL_MEMCPY((BytePointer)ptr + offset, &value, sizeof(T));
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

		void Print(std::ostream& os, int indent) const;
	};

	class Method : public Base
	{
	private:
		Type const* return_type;
		Parameter const* parameters;
		Size parameters_length;
		AccessSpecifier access_specifier;
		Linkage linkage;

	public:
		constexpr Method() :
			Base(kDefaultName),
			return_type(nullptr),
			parameters(nullptr),
			parameters_length(0),
			access_specifier(AccessSpecifier::kNone),
			linkage(Linkage::kNoLinkage)
		{}

		constexpr Method(
			char const* _name,
			Type const* _return_type,
			Parameter const* _parameters,
			Size _parameters_length,
			AccessSpecifier _access_specifier,
			Linkage _linkage
		) :
			Base(_name),
			return_type(_return_type),
			parameters(_parameters),
			parameters_length(_parameters_length),
			access_specifier(_access_specifier),
			linkage(_linkage)
		{}

		Linkage GetLinkage() const noexcept { return linkage; }
		AccessSpecifier GetAccessSpecifier() const noexcept { return access_specifier; }
		bool IsPublic() const noexcept { return access_specifier == AccessSpecifier::kPublic; }
		bool IsProtected() const noexcept { return access_specifier == AccessSpecifier::kProtected; }
		bool IsPrivate() const noexcept { return access_specifier == AccessSpecifier::kPrivate; }
		Type const* GetReturnType() const noexcept { return return_type; }
		Parameter const* GetParameter(Offset index) const noexcept { return &parameters[index]; }
		Size GetParameterLength() const noexcept { return parameters_length; }
		void Print(std::ostream& os, int indent) const;
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
	private:
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

	public:
		constexpr Type() :
			Base(kDefaultName),
			size(0),
			type_specifier_type(TypeSpecifierType::kBuiltin),
			ref_declarator(RefDeclarator::kNone),
			fields(nullptr),
			fields_length(0),
			methods(nullptr),
			methods_length(0),
			is_array(false),
			array_length(0),
			is_pointer(false),
			raw_type(nullptr)
		{}

		// array type ctor
		constexpr Type(
			char const* _name,
			size_t _size,
			TypeSpecifierType _type_specifier_type,
			bool _is_array,
			Size _array_length,
			Type const* _raw_type
		) :
			Base(_name),
			size(_size),
			type_specifier_type(_type_specifier_type),
			ref_declarator(RefDeclarator::kNone),
			fields(nullptr),
			fields_length(0),
			methods(nullptr),
			methods_length(0),
			is_array(_is_array),
			array_length(_array_length),
			is_pointer(false),
			raw_type(_raw_type)
		{}

		// pointer type ctor
		constexpr Type(
			char const* _name,
			size_t _size,
			TypeSpecifierType _type_specifier_type,
			bool _is_pointer,
			Type const* _raw_type
		) :
			Base(_name),
			size(_size),
			type_specifier_type(_type_specifier_type),
			ref_declarator(RefDeclarator::kNone),
			fields(nullptr),
			fields_length(0),
			methods(nullptr),
			methods_length(0),
			is_array(false),
			array_length(0),
			is_pointer(_is_pointer),
			raw_type(_raw_type)
		{}

		// reference type ctor
		constexpr Type(
			char const* _name,
			size_t _size,
			TypeSpecifierType _type_specifier_type,
			RefDeclarator _ref_declarator,
			Type const* _raw_type
		) :
			Base(_name),
			size(_size),
			type_specifier_type(_type_specifier_type),
			ref_declarator(_ref_declarator),
			fields(nullptr),
			fields_length(0),
			methods(nullptr),
			methods_length(0),
			is_array(false),
			array_length(0),
			is_pointer(false),
			raw_type(_raw_type)
		{}

		// builtin type ctor
		constexpr Type(
			char const* _name,
			size_t _size,
			TypeSpecifierType _type_specifier_type
		) :
			Base(_name),
			size(_size),
			type_specifier_type(_type_specifier_type),
			ref_declarator(RefDeclarator::kNone),
			fields(nullptr),
			fields_length(0),
			methods(nullptr),
			methods_length(0),
			is_array(false),
			array_length(0),
			is_pointer(false),
			raw_type(nullptr)
		{}

		// user type ctor
		constexpr Type(
			char const* _name,
			size_t _size,
			TypeSpecifierType _type_specifier_type,
			Field* _fields,
			Size _fields_length,
			Method* _methods,
			Size _methods_length
		) :
			Base(_name),
			size(_size),
			type_specifier_type(_type_specifier_type),
			ref_declarator(RefDeclarator::kNone),
			fields(_fields),
			fields_length(_fields_length),
			methods(_methods),
			methods_length(_methods_length),
			is_array(false),
			array_length(0),
			is_pointer(false),
			raw_type(nullptr)
		{}

		Type const* GetRawType() const noexcept { return raw_type; }
		Field* const GetField(Offset index) const noexcept { return &fields[index]; }
		Size GetFieldsLength() const noexcept { return fields_length; }
		Method* const GetMethod(Offset index) const noexcept { return &methods[index]; }
		Size GetMethodsLength() const noexcept { return methods_length; }
		TypeSpecifierType GetTypeSpecifierType() const { return type_specifier_type; }
		RefDeclarator GetRefDeclarator() const { return ref_declarator; }
		void Print(std::ostream& os, int indent) const;
	};

	template<typename T>
	Type const* GetType() noexcept;

	template<typename T>
	Type const* GetTypeImpl(Tag<T>) noexcept;

	template<typename T>
	Type const* GetType() noexcept { return GetTypeImpl(Tag<T>()); }

#define DECLARE_TYPE(T) \
	template<> \
	Type const* GetTypeImpl(Tag<T>) noexcept \
	{ \
		if (std::is_pointer<T>::value) \
		{ \
			static Type type(#T, sizeof(T), TypeSpecifierType::kBuiltin, true, GetType<std::remove_pointer<T>::type>()); \
			return &type; \
		} \
		else if (std::is_array<T>::value) \
		{ \
			static Type type(#T, sizeof(T), TypeSpecifierType::kBuiltin, true, std::extent<T>::value, GetType<std::remove_extent<T>::type>()); \
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

	// builtin types
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

	// print helpers
	static inline void PrintIndent(std::ostream& os, int indent)
	{
		for (int i = 0; i < indent; ++i)
		{
			os << " ";
		}
	}

	void Base::Print(std::ostream& os, int indent) const
	{
		PrintIndent(os, indent);
		os << "name: " << name << "\n";
	}

	void Parameter::Print(std::ostream& os, int indent) const
	{
		Base::Print(os, indent);
		PrintIndent(os, indent);
		os << "type: " << type->GetName() << "\n";
		PrintIndent(os, indent);
		os << "cvr qualifier: " << cvr_qualifier << "\n";
		PrintIndent(os, indent);
		os << "ref declarator: " << ref_declarator << "\n";
	}

	void Field::Print(std::ostream& os, int indent) const
	{
		Base::Print(os, indent);
		PrintIndent(os, indent);
		os << "type: " << type->GetName() << "\n";
		PrintIndent(os, indent);
		os << "offset: " << type->GetName() << "\n";
		PrintIndent(os, indent);
		os << "cvr qualifier: " << cvr_qualifier << "\n";
		PrintIndent(os, indent);
		os << "storage class specifier: " << storage_class_specifier << "\n";
		PrintIndent(os, indent);
		os << "thread storage class specifier: " << thread_storage_class_specifier << "\n";
		PrintIndent(os, indent);
		os << "storage duration: " << storage_duration << "\n";
		PrintIndent(os, indent);
		os << "access specifier: " << access_specifier << "\n";
	}

	void Method::Print(std::ostream& os, int indent) const
	{
		PrintIndent(os, indent);
		os << "method name: " << name << "\n";
		PrintIndent(os, indent);
		os << "return type name: " << return_type->GetName() << "\n";
		PrintIndent(os, indent);
		os << "access specifier: " << access_specifier << "\n";
		os << "linkage: " << linkage << "\n";
		PrintIndent(os, indent);
		os << "parameters length: " << parameters_length << "\n";
		PrintIndent(os, indent);
		os << "parameters: " << "\n";
		for (int i = 0; i < parameters_length; ++i)
		{
			auto param = parameters[i];
			PrintIndent(os, indent + 2);
			os << "parameters[" << i << "]:\n";
			param.Print(os, indent + 2);
		}
	}

	void Type::Print(std::ostream& os, int indent) const
	{
		Base::Print(os, indent);
		PrintIndent(os, indent);
		os << "size: " << size << "\n";
		PrintIndent(os, indent);
		os << "type specifier type: " << type_specifier_type << "\n";
		PrintIndent(os, indent);
		os << "ref declarator: " << ref_declarator << "\n";

		if (raw_type != nullptr)
		{
			PrintIndent(os, indent);
			os << "is array: " << is_array << ", array length: " << array_length << ", element type: " << raw_type->GetName() << "\n";
			PrintIndent(os, indent);
			os << "is pointer: " << is_pointer << ", pointee type: " << raw_type->GetName() << "\n";
		}

		PrintIndent(os, indent);
		os << "fields length:  " << fields_length << "\n";

		if (fields_length > 0)
		{
			PrintIndent(os, indent);
			os << "fields: " << "\n";
			for (int i = 0; i < fields_length; ++i)
			{
				auto field = fields[i];
				PrintIndent(os, indent + 2);
				os << "fields[" << i << "]:\n";
				field.Print(os, indent + 2);
			}
		}

		PrintIndent(os, indent);
		os << "methods length:  " << methods_length << "\n";

		if (methods_length > 0)
		{
			PrintIndent(os, indent);
			os << "methods: " << "\n";
			for (int i = 0; i < methods_length; ++i)
			{
				auto method = methods[i];
				PrintIndent(os, indent + 2);
				os << "methods[" << i << "]:\n";
				method.Print(os, indent + 2);
			}
		}
	}
}