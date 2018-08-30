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
#endif // CODEGEN VERSION CHECK

#ifndef TINYREFL_SEQUENCE
    #warning "The TINYREFL_SEQUENCE(...) macro is not defined. A definition of this macro is required by tinyrefl to model sequences of metadata"
    #warning "Tinyrefl will define an empty TINYREFL_SEQUENCE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_SEQUENCE(...)
#endif // TINYREFL_SEQUENCE

#if defined(TINYREFL_DEFINE_STRINGS) && !defined(TINYREFL_DEFINE_STRING)
    #warning "The TINYREFL_DEFINE_STRING(...) macro is not defined. A definition of this macro is required by tinyrefl to generate definitions of string constants"
    #warning "Tinyrefl will define an empty TINYREFL_DEFINE_STRING() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_DEFINE_STRING(...)
#endif // TINYREFL_DEFINE_STRING

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

#ifndef TINYREFL_ENUM_VALUE
    #warning "The TINYREFL_ENUM_VALUE(...) macro is not defined. A definition of this macro is required by tinyrefl to model sequences of metadata"
    #warning "Tinyrefl will define an empty TINYREFL_ENUM_VALUE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_ENUM_VALUE(...)
#endif // TINYREFL_ENUM_VALUE

#ifndef TINYREFL_CONSTRUCTOR
    #warning "The TINYREFL_CONSTRUCTOR(...) macro is not defined. A definition of this macro is required by tinyrefl to model class constructors"
    #warning "Tinyrefl will define an empty TINYREFL_CONSTRUCTOR() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_CONSTRUCTOR(...)
#endif // TINYREFL_CONSTRUCTOR

#ifndef TINYREFL_MEMBER_FUNCTION
    #warning "The TINYREFL_MEMBER_FUNCTION(...) macro is not defined. A definition of this macro is required by tinyrefl to model member functions"
    #warning "Tinyrefl will define an empty TINYREFL_MEMBER_FUNCTION() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_MEMBER_FUNCTION(...)
#endif // TINYREFL_MEMBER_FUNCTION

#ifndef TINYREFL_MEMBER_VARIABLE
    #warning "The TINYREFL_MEMBER_VARIABLE(...) macro is not defined. A definition of this macro is required by tinyrefl to model member variables"
    #warning "Tinyrefl will define an empty TINYREFL_MEMBER_VARIABLE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_MEMBER_VARIABLE(...)
#endif // TINYREFL_MEMBER_VARIABLE

#ifndef TINYREFL_REFLECT_MEMBER
    #warning "The TINYREFL_REFLECT_MEMBER(...) macro is not defined. A definition of this macro is required by tinyrefl to model sequences of metadata"
    #warning "Tinyrefl will define an empty TINYREFL_REFLECT_MEMBER() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REFLECT_MEMBER(...)
#endif // TINYREFL_REFLECT_MEMBER

#ifndef TINYREFL_REFLECT_CLASS
    #warning "The TINYREFL_REFLECT_CLASS(...) macro is not defined. A definition of this macro is required by tinyrefl to model the reflection metadata of a class"
    #warning "Tinyrefl will define an empty TINYREFL_REFLECT_CLASS() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REFLECT_CLASS(...)
#endif // TINYREFL_REFLECT_CLASS

#ifndef TINYREFL_GODMODE
    #warning "The TINYREFL_GODMODE(...) macro is not defined. A definition of this macro is required by tinyrefl to model universal access to class members, regardless of their access specifier"
    #warning "Tinyrefl will define an empty TINYREFL_GODMODE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_GODMODE(...)
#endif // TINYREFL_GODMODE

#ifndef TINYREFL_REFLECT_ENUM
    #warning "The TINYREFL_REFLECT_ENUM(...) macro is not defined. A definition of this macro is required by tinyrefl to model the reflection metadata of an enumeration"
    #warning "Tinyrefl will define an empty TINYREFL_REFLECT_ENUM() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_REFLECT_ENUM(...)
#endif // TINYREFL_REFLECT_ENUM
)========"
