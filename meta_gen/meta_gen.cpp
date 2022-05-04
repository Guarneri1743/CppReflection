#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include <unordered_map>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;

constexpr const char *kID = "id";
constexpr const char *kReflectAnnotation = "reflect";

static bool IsPredefinedType(QualType const &qualType) {
  auto type = qualType.split().Ty;
  return type->isConstantArrayType() || type->isReferenceType() ||
         type->isPointerType();
}

static std::string GetQualTypeQualifiedName(QualType const &qualType) {
  auto type = qualType.split().Ty;
  if (type->isBuiltinType()) {
    return type->getAs<BuiltinType>()
        ->getName(PrintingPolicy(LangOptions()))
        .str();
  } else if (type->isRecordType()) {
    return type->getAs<RecordType>()->getDecl()->getQualifiedNameAsString();
  } else if (type->isConstantArrayType()) {
    // todo: how to handle non-constant array?
    ConstantArrayType const *arrayType =
        cast<ConstantArrayType>(type->getAsArrayTypeUnsafe());
    auto size = arrayType->getSize();
    SmallString<255> S;
    size.toString(S, 10, true, false);
    return (arrayType->getElementType()
                ->getAsTagDecl()
                ->getQualifiedNameAsString() +
            "[" + S + "]")
        .str();
  } else if (type->isVoidType()) {
    return "void";
  } else if (type->isVoidPointerType()) {
    return "void*";
  } else if (type->isPointerType()) {
    return (GetQualTypeQualifiedName(type->getPointeeType()) + "*");
  } else if (type->isReferenceType()) {
    auto refType = type->getAs<ReferenceType>();
    return (GetQualTypeQualifiedName(refType->getPointeeType()) +
            (type->isLValueReferenceType() ? "&" : "&&"));
  } else {
    // to be implemented.
    return "Unknown";
  }
}

static std::string GetQualTypeName(QualType const &qualType) {
  auto type = qualType.split().Ty;
  if (type->isBuiltinType()) {
    return type->getAs<BuiltinType>()
        ->getName(PrintingPolicy(LangOptions()))
        .str();
  } else if (type->isRecordType()) {
    return type->getAs<RecordType>()->getDecl()->getName().str();
  } else if (type->isConstantArrayType()) {
    // todo: how to handle non-constant array?
    ConstantArrayType const *arrayType =
        cast<ConstantArrayType>(type->getAsArrayTypeUnsafe());
    auto size = arrayType->getSize();
    SmallString<255> S;
    size.toString(S, 10, true, false);
    return (arrayType->getElementType()->getAsTagDecl()->getName() + "[" + S +
            "]")
        .str();
  } else if (type->isVoidType()) {
    return "void";
  } else if (type->isVoidPointerType()) {
    return "void*";
  } else if (type->isPointerType()) {
    return (GetQualTypeName(type->getPointeeType()) + "*");
  } else if (type->isReferenceType()) {
    auto refType = type->getAs<ReferenceType>();
    return (GetQualTypeName(refType->getPointeeType()) +
            (type->isLValueReferenceType() ? "&" : "&&"));
  } else {
    // to be implemented.
    return "Unknown";
  }
}

static raw_ostream &PrintCVRQualifier(raw_ostream &os, unsigned TypeQuals) {
  bool appendSpace = false;
  if (TypeQuals & Qualifiers::Const) {
    os << "CVRQualifier::kConst";
    appendSpace = true;
  }
  if (TypeQuals & Qualifiers::Volatile) {
    if (appendSpace)
      os << " | ";
    os << "CVRQualifier::kVolatile";
    appendSpace = true;
  }
  if (TypeQuals & Qualifiers::Restrict) {
    if (appendSpace)
      os << " | ";
    os << "CVRQualifier::kRestrict";
  }

  return os;
}

static raw_ostream &PrintCVRQualifier(raw_ostream &os, ValueDecl const *decl) {
  if (decl) {
    SplitQualType Split = decl->getType().split();
    Qualifiers Quals = Split.Quals;
    if (const auto *Subst = dyn_cast<SubstTemplateTypeParmType>(Split.Ty))
      Quals -= QualType(Subst, 0).getQualifiers();
    if (!Quals.empty()) {
      PrintCVRQualifier(os, Quals.getCVRQualifiers());
    } else {
      os << "CVRQualifier::kNone";
    }
  }
  return os;
}

static raw_ostream &PrintTypeSpecifierType(raw_ostream &os,
                                           clang::Type const *type) {
  if (type->isBuiltinType()) {
    os << "TypeSpecifierType::kBuiltin";
    return os;
  } else if (type->isStructuralType()) {
    os << "TypeSpecifierType::kStruct";
    return os;
  } else if (type->isUnionType()) {
    os << "TypeSpecifierType::kUnion";
    return os;
  } else if (type->isClassType()) {
    os << "TypeSpecifierType::kClass";
    return os;
  } else if (type->isEnumeralType()) {
    os << "TypeSpecifierType::kEnum";
    return os;
  }

  llvm_unreachable("Invalid type specifier type");
  return os;
}

static raw_ostream &PrintRefDeclarator(raw_ostream &os,
                                       clang::Type const *type) {
  if (type) {
    if (type->isLValueReferenceType()) {
      os << "RefDeclarator::kLValueReference";
      return os;
    } else if (type->isRValueReferenceType()) {
      os << "RefDeclarator::kRValueReference";
      return os;
    } else {
      os << "RefDeclarator::kNone";
      return os;
    }
  }
  return os;
}

static raw_ostream &PrintStorageClassSpecifier(raw_ostream &os,
                                               VarDecl const *decl) {
  if (decl) {
    switch (decl->getStorageClass()) {
    case SC_None:
      os << "StorageClassSpecifier::kNone";
      break;
    case SC_Auto:
      os << "StorageClassSpecifier::kAuto";
      break;
    case SC_Extern:
      os << "StorageClassSpecifier::kExtern";
      break;
    case SC_PrivateExtern:
      os << "StorageClassSpecifier::kPrivateExtern";
      break;
    case SC_Register:
      os << "StorageClassSpecifier::kRegister";
      break;
    case SC_Static:
      os << "StorageClassSpecifier::kStatic";
      break;
    default:
      llvm_unreachable("Invalid storage class specifier");
      break;
    }
  }

  return os;
}

static raw_ostream &PrintStorageDuration(raw_ostream &os, VarDecl const *decl) {
  if (decl) {
    switch (decl->getStorageDuration()) {
    case SD_FullExpression:
      os << "StorageDuration::kFullExpression";
      break;
    case SD_Automatic:
      os << "StorageDuration::kAutomatic";
      break;
    case SD_Thread:
      os << "StorageDuration::kThread";
      break;
    case SD_Static:
      os << "StorageDuration::kStatic";
      break;
    case SD_Dynamic:
      os << "StorageDuration::kDynamic";
      break;
    default:
      llvm_unreachable("Invalid storage duration");
      break;
    }
  }

  return os;
}

static raw_ostream &PrintThreadStorageClassSpecifier(raw_ostream &os,
                                                     VarDecl const *decl) {
  if (decl) {
    switch (decl->getTSCSpec()) {
    case TSCS_unspecified:
      os << "ThreadStorageClassSpecifier::kUnSpecified";
      break;
    case TSCS___thread:
      os << "ThreadStorageClassSpecifier::kGNUThread";
      break;
    case TSCS_thread_local:
      os << "ThreadStorageClassSpecifier::kCXX11ThreadLocal";
      break;
    case TSCS__Thread_local:
      os << "ThreadStorageClassSpecifier::kC11ThreadLocal";
      break;
    default:
      llvm_unreachable("Invalid thread storage class specifier");
      break;
    }
  }

  return os;
}

static raw_ostream &PrintAccessSpecifier(raw_ostream &os, Decl const *decl) {
  if (decl) {
    switch (decl->getAccess()) {
    case AccessSpecifier::AS_none:
      os << "AccessSpecifier::kNone";
      break;
    case AccessSpecifier::AS_private:
      os << "AccessSpecifier::kPrivate";
      break;
    case AccessSpecifier::AS_protected:
      os << "AccessSpecifier::kProtected";
      break;
    case AccessSpecifier::AS_public:
      os << "AccessSpecifier::kPublic";
      break;
    default:
      llvm_unreachable("Invalid thread access specifier");
      break;
    }
  }
  return os;
}

static raw_ostream &PrintLinkage(raw_ostream &os,
                                 clang::FunctionDecl const *decl) {
  if (decl) {
    switch (decl->getLinkageAndVisibility().getLinkage()) {
    case Linkage::NoLinkage:
      os << "Linkage::kNoLinkage";
      break;
    case Linkage::InternalLinkage:
      os << "Linkage::kInternalLinkage";
      break;
    case Linkage::UniqueExternalLinkage:
      os << "Linkage::kUniqueExternalLinkage";
      break;
    case Linkage::VisibleNoLinkage:
      os << "Linkage::kVisibleNoLinkage";
      break;
    case Linkage::ModuleInternalLinkage:
      os << "Linkage::kModuleInternalLinkage";
      break;
    case Linkage::ModuleLinkage:
      os << "Linkage::kModuleLinkage";
      break;
    case Linkage::ExternalLinkage:
      os << "Linkage::kExternalLinkage";
      break;
    default:
      llvm_unreachable("Invalid linkage");
      break;
    }
  }
  return os;
}

static void PrintIndent(raw_ostream &os, int count) {
  for (int i = 0; i < count; ++i)
    os << "\t";
}

static void PrintField(raw_ostream &os, int indent, SmallString<64> &type,
                       FieldDecl const *decl, int index) {
  // static Type field_0_Type = *GetType<Bar>();
  PrintIndent(os, indent);
  os << "static Type "
     << "field_" << index << "_Type"
     << " = *GetType<" << GetQualTypeQualifiedName(decl->getType()) << ">();\n";

  PrintIndent(os, indent);
  // typeStorage.fields[0] = Reflection::Field(
  os << "typeStorage.fields[" << index << "] = Reflection::Field(";
  // name
  os << "\"" << decl->getQualifiedNameAsString() << "\"";
  os << ", ";
  // type
  os << "&field_" << index << "_Type";
  os << ", ";
  // offset
  os << "offsetof(" << type << ", " << decl->getQualifiedNameAsString() << ")";
  os << ", ";
  // CVRQualifier
  PrintCVRQualifier(os, decl);
  os << ", ";
  // StorageClassSpecifier
  os << "StorageClassSpecifier::kNone";
  os << ", ";
  // ThreadStorageClassSpecifier
  os << "ThreadStorageClassSpecifier::kUnSpecified";
  os << ", ";
  // StorageDuration
  os << "StorageDuration::kNone";
  os << ", ";
  // AccessSpecifier
  PrintAccessSpecifier(os, decl);
  os << ");\n";
}

static void PrintField(raw_ostream &os, int indent, SmallString<64> &type,
                       VarDecl const *decl, int index) {
  // static Type field_0_Type = *GetType<Bar>();
  PrintIndent(os, indent);
  os << "static Type "
     << "field_" << index << "_Type"
     << " = *GetType<" << GetQualTypeQualifiedName(decl->getType()) << ">();\n";

  PrintIndent(os, indent);

  // typeStorage.fields[0] = Reflection::Field(
  os << "typeStorage.fields[" << index << "] = Reflection::Field(";

  // name
  os << "\"" << decl->getQualifiedNameAsString() << "\"";
  os << ", ";

  // type
  os << "&field_" << index << "_Type";
  os << ", ";

  // offset
  if (!decl->isStaticDataMember()) {
    os << "offsetof(" << type << ", " << decl->getQualifiedNameAsString()
       << ")";
  } else {
    os << "0";
  }
  os << ", ";

  // CVRQualifier
  PrintCVRQualifier(os, decl);
  os << ", ";

  // StorageClassSpecifier
  PrintStorageClassSpecifier(os, decl);
  os << ", ";

  // ThreadStorageClassSpecifier
  PrintThreadStorageClassSpecifier(os, decl);
  os << ", ";

  // StorageDuration
  PrintStorageDuration(os, decl);

  os << ", ";
  // AccessSpecifier
  PrintAccessSpecifier(os, decl);

  // );
  os << ");\n";
}

static void PrintParameter(raw_ostream &os, int indent, SmallString<64> &type,
                           FunctionDecl const *decl, int index,
                           clang::ParmVarDecl const *param,
                           unsigned int paramIndex) {
  // static Type param_0_Type = *GetType<int>();
  PrintIndent(os, indent);
  os << "static Type "
     << "method_" << index << "_param_" << paramIndex << "_type = *GetType <"
     << GetQualTypeQualifiedName(param->getType()) << ">();\n";

  PrintIndent(os, indent);
  // method_0_parameters[0] = Reflection::Parameter(;
  os << "method_" << index << "_parameters[" << paramIndex
     << "] = Reflection::Parameter(";

  // name
  os << "\"" << param->getQualifiedNameAsString() << "\"";
  os << ", ";

  // type
  os << "&method_" << index << "_param_" << paramIndex << "_type";
  os << ", ";

  // CVRQualifier
  PrintCVRQualifier(os, param);
  os << ", ";

  // RefDeclarator
  PrintRefDeclarator(os, &(*param->getType()));

  // );
  os << ");\n";
}

static void PrintMethod(raw_ostream &os, int indent, SmallString<64> &type,
                        FunctionDecl const *decl, int index) {
  auto methodName = decl->getQualifiedNameAsString();

  // static Type method_0_ret_type = *GetType<int>();
  PrintIndent(os, indent);
  os << "static Type method_" << index << "_ret_type = *GetType<"
     << GetQualTypeQualifiedName(decl->getReturnType()) << ">();\n";

  // parameters
  if (decl->getNumParams() > 0) {
    PrintIndent(os, indent);
    os << "static Parameter "
       << "method_" << index << "_parameters[" << decl->getNumParams()
       << "];\n";

    for (unsigned int paramIndex = 0; paramIndex < decl->getNumParams(); ++paramIndex) {
      clang::ParmVarDecl const *param = decl->getParamDecl(paramIndex);
      PrintParameter(os, indent, type, decl, index, param, paramIndex);
    }
  }

  // typeStorage.methods[index] = Reflection::Method(
  PrintIndent(os, indent);
  os << "typeStorage.methods[" << index << "] = Reflection::Method(";

  // name
  os << "\"" << methodName << "\"";
  os << ", ";

  // return type
  os << "&method_" << index << "_ret_type";
  os << ", ";

  // parameters
  if (decl->getNumParams() > 0) {
    os << "&method_" << index << "_parameters[0]";
  } else {
    os << "nullptr";
  }
  os << ", ";

  os << decl->getNumParams();
  os << ", ";

  // AccessSpecifier
  PrintAccessSpecifier(os, decl);
  os << ", ";

  // Linkage
  PrintLinkage(os, decl);

  // );
  os << ");\n";
}

static void PrintType(raw_ostream &os, int indent,

                      RecordDecl const *decl, SmallString<64> &type,
                      std::vector<FieldDecl const *> const &fields,
                      std::vector<VarDecl const *> const &var_fields,
                      std::vector<FunctionDecl const *> const &methods) {
  PrintIndent(os, indent);
  os << "template<>\n";
  PrintIndent(os, indent);
  os << "Type const* GetTypeImpl(Tag<" << type << ">) noexcept\n";
  PrintIndent(os, indent);
  os << "{\n";
  indent++;
  PrintIndent(os, indent);
  os << "static TypeStorage<" << type << ", "
     << fields.size() + var_fields.size() << ", " << methods.size()
     << "> typeStorage;\n";

  int fieldIndex = 0;

  for (auto &field : fields) {
    PrintField(os, indent, type, field, fieldIndex++);
  }

  for (auto &field : var_fields) {
    PrintField(os, indent, type, field, fieldIndex++);
  }

  int methodIndex = 0;
  for (auto &method : methods) {
    PrintMethod(os, indent, type, method, methodIndex++);
  }

  // static Type type("int", sizeof(int),
  PrintIndent(os, indent);
  os << "static Type type(\"" << type << "\", sizeof(" << type << "), ";
  // TypeSpecifierType,
  PrintTypeSpecifierType(os, decl->getTypeForDecl());
  os << ", typeStorage.fields, typeStorage.kFieldsNum, typeStorage.methods, "
        "typeStorage.kMethodsNum);\n";

  // return &type;
  PrintIndent(os, indent);
  os << "return &type;\n";

  // }
  indent--;
  PrintIndent(os, indent);
  os << "};\n\n";
}

static std::unordered_map<std::string, int> name2PredefinedType;

static bool PrintPredefinedType(raw_ostream &os, int indent,
                                QualType const &type) {
  if (!IsPredefinedType(type)) {
    return false;
  }
  auto qualifiedName = GetQualTypeQualifiedName(type);
  if (name2PredefinedType.count(qualifiedName) <= 0) {
    name2PredefinedType[qualifiedName] = 1;
    PrintIndent(os, indent);
    os << "DECLARE_TYPE(" << qualifiedName << ");\n";
    return true;
  }
  return false;
}

class ASTResult {
private:
  CXXRecordDecl const *record;
  std::vector<FieldDecl const *> fields;
  std::vector<VarDecl const *> varFields;
  std::vector<FunctionDecl const *> methods;
  std::unordered_map<std::string, size_t> name2field;
  std::unordered_map<std::string, size_t> name2method;

public:
  ASTResult(CXXRecordDecl const *_record) : record(_record) {}

  void AddField(FieldDecl const *field) {
    std::string name = field->getQualifiedNameAsString();
    if (name2field.count(name) > 0) {
      return;
    }
    name2field[name] = fields.size();
    fields.push_back(field);
  }

  void AddStaticField(VarDecl const *field) {
    std::string name = field->getQualifiedNameAsString();
    if (name2field.count(name) > 0) {
      return;
    }
    name2field[name] = fields.size();
    varFields.push_back(field);
  }

  void AddMethod(FunctionDecl const *method) {
    std::string name = method->getQualifiedNameAsString();
    if (name2method.count(name) > 0) {
      return;
    }
    name2method[name] = methods.size();
    methods.push_back(method);
  }

  void PrintPredefinedTypes(raw_ostream &os, int indent) {
    int count = 0;
    for (auto &field : fields) {
      auto type = field->getType();
      if (PrintPredefinedType(os, indent, type)) {
        count++;
      }
    }

    for (auto &field : varFields) {
      auto type = field->getType();
      if (PrintPredefinedType(os, indent, type)) {
        count++;
      }
    }

    for (auto &method : methods) {
      for (unsigned int i = 0; i < method->getNumParams(); ++i) {
        auto type = method->getParamDecl(i)->getType();
        if (PrintPredefinedType(os, indent, type)) {
          count++;
        }
      }
    }

    os << " \n";
  }

  void Print(raw_ostream &os) {
    SmallString<64> type;
    raw_svector_ostream stos(type);
    record->printQualifiedName(stos);
    PrintPredefinedTypes(os, 1);
    PrintType(os, 1, record, type, fields, varFields, methods);
  }
};

class AnnotationFinder : public MatchFinder::MatchCallback {
public:
  template <unsigned N>
  StringRef GetAnnotations(Attr const *attr, SmallString<N> &str) {
    str.clear();
    raw_svector_ostream os(str);
    LangOptions langopts;
    PrintingPolicy policy(langopts);
    attr->printPretty(os, policy);
    return str.slice(26, str.size() - 4);
  }

  bool HasAnnotation(clang::AttrVec const &attrs, llvm::StringRef keyword) {
    for (auto &attr : attrs) {
      if (attr->getKind() == attr::Kind::Annotate) {
        SmallString<64> str;
        auto annotationStr = GetAnnotations(attr, str);
        if (annotationStr.startswith(keyword)) {
          return true;
        }
      }
    }

    return false;
  }

  virtual void run(MatchFinder::MatchResult const &result) override {
    CXXRecordDecl const *record = result.Nodes.getNodeAs<CXXRecordDecl>(kID);
    if (record) {
      records.emplace_back(record);
      astContext = result.Context;
      fileName = (std::string)result.SourceManager->getFilename(
          result.SourceManager->getFileLoc(record->getLocation()));
      return;
    }

    FunctionDecl const *methodNode = result.Nodes.getNodeAs<FunctionDecl>(kID);
    if (methodNode) {
      if (methodNode->hasAttr<AnnotateAttr>() &&
          HasAnnotation(methodNode->getAttrs(), kReflectAnnotation)) {
        records.back().AddMethod(methodNode);
      }
      return;
    }

    FieldDecl const *fieldNode = result.Nodes.getNodeAs<FieldDecl>(kID);
    if (fieldNode) {
      if (fieldNode->hasAttr<AnnotateAttr>() &&
          HasAnnotation(fieldNode->getAttrs(), kReflectAnnotation)) {
        records.back().AddField(fieldNode);
      }
      return;
    }

    VarDecl const *varNode = result.Nodes.getNodeAs<VarDecl>(kID);
    if (varNode) {
      if (varNode->hasAttr<AnnotateAttr>() &&
          HasAnnotation(varNode->getAttrs(), kReflectAnnotation)) {
        records.back().AddStaticField(varNode);
      }
      return;
    }
  }

  void PrintHeader(raw_ostream &os) {
    os << "// auto-generated file.\n";
    os << "#pragma once\n";
    os << "#include \"reflection.hpp\"\n";
    os << "\n\n\n";
  }

  void PrintNamespace(raw_ostream &os) {
    os << "namespace Reflection\n";
    os << "{\n";
  }

  void PrintEndNamespace(raw_ostream &os) { os << "}\n"; }

  virtual void onEndOfTranslationUnit() override {
    std::error_code error;
    std::string fileNameWithoutExt = fileName.substr(0, fileName.rfind("."));
    fileNameWithoutExt.append("_gen_refl.h");
    llvm::outs() << fileNameWithoutExt << " generated.\n";
    llvm::raw_fd_ostream os(fileNameWithoutExt, error);

    PrintHeader(os);
    PrintNamespace(os);
    for (auto &record : records) {
      record.Print(os);
    }
    PrintEndNamespace(os);
  }

private:
  ASTContext const *astContext;
  std::string fileName;
  std::vector<ASTResult> records;
};

static llvm::cl::OptionCategory optionCategory("ast options");

int main(int argc, const char **argv) {
  auto optionsParser = CommonOptionsParser::create(argc, argv, optionCategory);
  ClangTool tool(optionsParser->getCompilations(),
                 optionsParser->getSourcePathList());

  AnnotationFinder annotationFinder;
  MatchFinder finder;

  static DeclarationMatcher const typeMatcher =
      cxxRecordDecl(decl().bind(kID), hasAttr(attr::Annotate));
  finder.addMatcher(typeMatcher, &annotationFinder);

  static DeclarationMatcher const fieldMatcher =
      fieldDecl(decl().bind(kID), hasAttr(attr::Annotate));
  finder.addMatcher(fieldMatcher, &annotationFinder);

  static DeclarationMatcher const methodMatcher =
      functionDecl(decl().bind(kID), hasAttr(attr::Annotate));
  finder.addMatcher(methodMatcher, &annotationFinder);

  static DeclarationMatcher const staticMatcher =
      varDecl(decl().bind(kID), hasAttr(attr::Annotate));
  finder.addMatcher(staticMatcher, &annotationFinder);

  return tool.run(newFrontendActionFactory(&finder).get());
}