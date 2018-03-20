R"========(
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

#ifndef TINYREFL_ENUM_VALUE
    #warning "The TINYREFL_ENUM_VALUE(...) macro is not defined. A definition of this macro is required by tinyrefl to model sequences of metadata"
    #warning "Tinyrefl will define an empty TINYREFL_ENUM_VALUE() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_ENUM_VALUE(...)
#endif // TINYREFL_ENUM_VALUE

#ifndef TINYREFL_MEMBER
    #warning "The TINYREFL_MEMBER(...) macro is not defined. A definition of this macro is required by tinyrefl to model sequences of metadata"
    #warning "Tinyrefl will define an empty TINYREFL_MEMBER() macro for you, but this would mean the metadata of your types could end up being incomplete"

    #define TINYREFL_MEMBER(...)
#endif // TINYREFL_MEMBER

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
