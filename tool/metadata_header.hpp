R"========(

#ifndef TINYREFL_API_CODEGEN_VERSION_MAJOR
    #error "TINYREFL_API_CODEGEN_VERSION_MAJOR macro not found. API backends must define this macro with the generated code major version they support"
#endif // TINYREFL_API_CODEGEN_VERSION_MAJOR

#ifndef TINYREFL_API_CODEGEN_VERSION_MINOR
    #error "TINYREFL_API_CODEGEN_VERSION_MINOR macro not found. API backends must define this macro with the generated code minor version they support"
#endif // TINYREFL_API_CODEGEN_VERSION_MINOR

#ifndef TINYREFL_API_CODEGEN_VERSION_FIX
    #warning "TINYREFL_API_CODEGEN_VERSION_FIX macro not found. API backends are encouraged to define this macro with the generated code fix version they support"
    #define TINYREFL_API_CODEGEN_VERSION_FIX 0
#endif // TINYREFL_API_CODEGEN_VERSION_FIX

#ifdef TINYREFL_API_CODEGEN_VERSION
    #undef TINYREFL_API_CODEGEN_VERSION
#endif // TINYREFL_API_CODEGEN_VERSION

#ifdef TINYREFL_MAKE_API_CODEGEN_VERSION
    #undef TINYREFL_MAKE_API_CODEGEN_VERSION
#endif // TINYREFL_MAKE_API_CODEGEN_VERSION

#ifdef TINYREFL_MAKE_API_CODEGEN_VERSION_STRING
    #undef TINYREFL_MAKE_API_CODEGEN_VERSION_STRING
#endif // TINYREFL_MAKE_API_CODEGEN_VERSION_STRING

#define TINYREFL_MAKE_API_CODEGEN_VERSION_STRING(...) # __VA_ARGS__
#define TINYREFL_MAKE_API_CODEGEN_VERSION(major, minor, fix) \
    TINYREFL_MAKE_API_CODEGEN_VERSION_STRING(major) "." TINYREFL_MAKE_API_CODEGEN_VERSION_STRING(minor) "." TINYREFL_MAKE_API_CODEGEN_VERSION_STRING(fix)

#define TINYREFL_API_CODEGEN_VERSION TINYREFL_MAKE_API_CODEGEN_VERSION(TINYREFL_API_CODEGEN_VERSION_MAJOR, TINYREFL_API_CODEGEN_VERSION_MINOR, TINYREFL_API_CODEGEN_VERSION_FIX)
#define TINYREFL_API_CODEGEN_VERSION_SOFT TINYREFL_MAKE_API_CODEGEN_VERSION(TINYREFL_API_CODEGEN_VERSION_MAJOR, TINYREFL_API_CODEGEN_VERSION_MINOR, x)

#if ((TINYREFL_TOOL_CODEGEN_VERSION_MAJOR != TINYREFL_API_CODEGEN_VERSION_MAJOR) || \
     (TINYREFL_TOOL_CODEGEN_VERSION_MINOR != TINYREFL_API_CODEGEN_VERSION_MINOR))
    #error "Tinyrefl tool generated code is not compatible with your API. Both major and minor version must be equal"
    #error "Tinyrefl tool generated code version is " TINYREFL_TOOL_CODEGEN_VERSION
    #error "Your API expects generated code from version " TINYREFL_API_CODEGEN_VERSION_SOFT
#endif // oCODEGEN VERSION CHECK

// basic metadata types

#ifndef TINYREFL_SEQUENCE
    #warning "The TINYREFL_SEQUENCE(...) macro is not defined. A definition of this macro is required by tinyrefl to model sequences of metadata"
    #warning "Tinyrefl will define an empty TINYREFL_SEQUENCE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_SEQUENCE(...)
#endif // TINYREFL_SEQUENCE

#if !defined(TINYREFL_REGISTER_STRING)
    #warning "The TINYREFL_REGISTER_STRING(...) macro is not defined. A definition of this macro is required by tinyrefl to generate definitions of string constants"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_STRING() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_STRING(...)
#endif // TINYREFL_REGISTER_STRING

#ifndef TINYREFL_STRING
    #warning "The TINYREFL_STRING(...) macro is not defined. A definition of this macro is required by tinyrefl to model string constants"
    #warning "Tinyrefl will define an empty TINYREFL_STRING() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_STRING(...)
#endif // TINYREFL_STRING

#ifndef TINYREFL_TYPE
    #warning "The TINYREFL_TYPE(...) macro is not defined. A definition of this macro is required by tinyrefl to model references to types"
    #warning "Tinyrefl will define an empty TINYREFL_TYPE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_TYPE(...)
#endif // TINYREFL_TYPE

#ifndef TINYREFL_VALUE
    #warning "The TINYREFL_VALUE(...) macro is not defined. A definition of this macro is required by tinyrefl to model values (Such as integral constants, string literals, etc)"
    #warning "Tinyrefl will define an empty TINYREFL_VALUE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_VALUE(...)
#endif // TINYREFL_VALUE

#ifndef TINYREFL_ATTRIBUTE
    #warning "The TINYREFL_ATTRIBUTE(...) macro is not defined. A definition of this macro is required by tinyrefl to model user defined attributes"
    #warning "Tinyrefl will define an empty TINYREFL_ATTRIBUTE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_ATTRIBUTE(...)
#endif // TINYREFL_ATTRIBUTE

// entities

#ifndef TINYREFL_FILE
    #warning "The TINYREFL_FILE(...) macro is not defined. A definition of this macro is required by tinyrefl to model reflection metadata of a header file"
    #warning "Tinyrefl will define an empty TINYREFL_FILE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_FILE(...)
#endif // TINYREFL_FILE

#ifndef TINYREFL_NAMESPACE
    #warning "The TINYREFL_NAMESPACE(...) macro is not defined. A definition of this macro is required by tinyrefl to model namespaces"
    #warning "Tinyrefl will define an empty TINYREFL_NAMESPACE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_NAMESPACE(...)
#endif // TINYREFL_NAMESPACE

#ifndef TINYREFL_FUNCTION
    #warning "The TINYREFL_FUNCTION(...) macro is not defined. A definition of this macro is required by tinyrefl to model non member functions"
    #warning "Tinyrefl will define an empty TINYREFL_FUNCTION() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_FUNCTION(...)
#endif // TINYREFL_FUNCTION

#ifndef TINYREFL_VARIABLE
    #warning "The TINYREFL_VARIABLE(...) macro is not defined. A definition of this macro is required by tinyrefl to model non member variables"
    #warning "Tinyrefl will define an empty TINYREFL_VARIABLE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_VARIABLE(...)
#endif // TINYREFL_VARIABLE

#ifndef TINYREFL_ENUM
    #warning "The TINYREFL_ENUM(...) macro is not defined. A definition of this macro is required by tinyrefl to model enum types"
    #warning "Tinyrefl will define an empty TINYREFL_ENUM() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_ENUM(...)
#endif // TINYREFL_ENUM

#ifndef TINYREFL_ENUM_VALUE
    #warning "The TINYREFL_ENUM_VALUE(...) macro is not defined. A definition of this macro is required by tinyrefl to model enum values"
    #warning "Tinyrefl will define an empty TINYREFL_ENUM_VALUE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_ENUM_VALUE(...)
#endif // TINYREFL_ENUM_VALUE

#ifndef TINYREFL_CLASS
    #warning "The TINYREFL_CLASS(...) macro is not defined. A definition of this macro is required by tinyrefl to model class types"
    #warning "Tinyrefl will define an empty TINYREFL_CLASS() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_CLASS(...)
#endif // TINYREFL_CLASS

#ifndef TINYREFL_CONSTRUCTOR
    #warning "The TINYREFL_CONSTRUCTOR(...) macro is not defined. A definition of this macro is required by tinyrefl to model class constructors"
    #warning "Tinyrefl will define an empty TINYREFL_CONSTRUCTOR() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_CONSTRUCTOR(...)
#endif // TINYREFL_CONSTRUCTOR

#ifndef TINYREFL_MEMBER_FUNCTION
    #warning "The TINYREFL_MEMBER_FUNCTION(...) macro is not defined. A definition of this macro is required by tinyrefl to model non static member functions"
    #warning "Tinyrefl will define an empty TINYREFL_MEMBER_FUNCTION() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_MEMBER_FUNCTION(...)
#endif // TINYREFL_MEMBER_FUNCTION

#ifndef TINYREFL_MEMBER_VARIABLE
    #warning "The TINYREFL_MEMBER_VARIABLE(...) macro is not defined. A definition of this macro is required by tinyrefl to model non static member variables"
    #warning "Tinyrefl will define an empty TINYREFL_MEMBER_VARIABLE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_MEMBER_VARIABLE(...)
#endif // TINYREFL_MEMBER_VARIABLE

// registrations

#ifndef TINYREFL_REGISTER_FILE
    #warning "The TINYREFL_REGISTER_MEMBER_VARIABLE(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a header file"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_FILE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_FILE(...)
#endif // TINYREFL_REGISTER_FILE

#ifndef TINYREFL_REGISTER_NAMESPACE
    #warning "The TINYREFL_REGISTER_NAMESPACE(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a namespace"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_NAMESPACE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_NAMESPACE(...)
#endif // TINYREFL_REGISTER_NAMESPACE

#ifndef TINYREFL_REGISTER_FUNCTION
    #warning "The TINYREFL_REGISTER_FUNCTION(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a function"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_FUNCTION() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_FUNCTION(...)
#endif // TINYREFL_REGISTER_FUNCTION

#ifndef TINYREFL_REGISTER_VARIABLE
    #warning "The TINYREFL_REGISTER_VARIABLE(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a variable"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_VARIABLE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_VARIABLE(...)
#endif // TINYREFL_REGISTER_VARIABLE

#ifndef TINYREFL_REGISTER_ENUM
    #warning "The TINYREFL_REGISTER_ENUM(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of anum types"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_ENUM() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_ENUM(...)
#endif // TINYREFL_REGISTER_ENUM

#ifndef TINYREFL_REGISTER_ENUM_VALUE
    #warning "The TINYREFL_REGISTER_ENUM_VALUE(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of enum values"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_ENUM_VALUE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_ENUM_VALUE(...)
#endif // TINYREFL_REGISTER_ENUM_VALUE

#ifndef TINYREFL_REGISTER_CLASS
    #warning "The TINYREFL_REGISTER_CLASS(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a class"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_CLASS() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_CLASS(...)
#endif // TINYREFL_REGISTER_CLASS

#ifndef TINYREFL_STATIC_MEMBER_FUNCTION
    #warning "The TINYREFL_STATIC_MEMBER_FUNCTION(...) macro is not defined. A definition of this macro is required by tinyrefl to model static member functions"
    #warning "Tinyrefl will define an empty TINYREFL_STATIC_MEMBER_FUNCTION() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_STATIC_MEMBER_FUNCTION(...)
#endif // TINYREFL_STATIC_MEMBER_FUNCTION

#ifndef TINYREFL_STATIC_MEMBER_VARIABLE
    #warning "The TINYREFL_STATIC_MEMBER_VARIABLE(...) macro is not defined. A definition of this macro is required by tinyrefl to model static member variables"
    #warning "Tinyrefl will define an empty TINYREFL_STATIC_MEMBER_VARIABLE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_STATIC_MEMBER_VARIABLE(...)
#endif // TINYREFL_STATIC_MEMBER_VARIABLE

#ifndef TINYREFL_REGISTER_MEMBER_FUNCTION
    #warning "The TINYREFL_REGISTER_FUNCTION(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a function"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_MEMBER_FUNCTION() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_MEMBER_FUNCTION(...)
#endif // TINYREFL_REGISTER_MEMBER_FUNCTION

#ifndef TINYREFL_REGISTER_MEMBER_VARIABLE
    #warning "The TINYREFL_REGISTER_VARIABLE(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a variable"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_MEMBER_VARIABLE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_MEMBER_VARIABLE(...)
#endif // TINYREFL_REGISTER_MEMBER_VARIABLE

#ifndef TINYREFL_REGISTER_STATIC_MEMBER_FUNCTION
    #warning "The TINYREFL_REGISTER_STATIC_MEMBER_FUNCTION(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a static member function"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_STATIC_MEMBER_FUNCTION() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_STATIC_MEMBER_FUNCTION(...)
#endif // TINYREFL_REGISTER_STATIC_MEMBER_FUNCTION

#ifndef TINYREFL_REGISTER_STATIC_MEMBER_VARIABLE
    #warning "The TINYREFL_REGISTER_STATIC_MEMBER_VARIABLE(...) macro is not defined. A definition of this macro is required by tinyrefl to model the registration of reflection metadata of a static member variable"
    #warning "Tinyrefl will define an empty TINYREFL_REGISTER_STATIC_MEMBER_VARIABLE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REGISTER_STATIC_MEMBER_VARIABLE(...)
#endif // TINYREFL_REGISTER_STATIC_MEMBER_VARIABLE
)========"
