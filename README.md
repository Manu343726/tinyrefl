tinyrefl [![pipeline status](https://gitlab.com/Manu343726/tinyrefl/badges/master/pipeline.svg)](https://gitlab.com/Manu343726/tinyrefl/commits/master) [![Build status](https://ci.appveyor.com/api/projects/status/ht9h9sq6hfvpn6hg?svg=true)](https://ci.appveyor.com/project/Manu343726/tinyrefl)
========

A work in progress minimal C++ static reflection API and codegen tool.

## Features

 - C++14 static reflection API: Convert enum values to strings, get the set of member
   functions of a class, check for attributes, visit object members...

   ``` cpp
   // foo.h

   namespace mylib {
   class [[interesting]] Foo {
       std::string str;
       void f() {}
       int i;

       enum class Enum
       {
           A, B, C
       };
   };
   }

   // main.cpp

   #include <tinyrefl/api.hpp>
   #include "foo.h"
   #include "foo.h.tinyrefl"

   int main()
   {
       static_assert(tinyrefl::has_attribute<mylib::Foo>("interesting"),
           "Wow, I thought your API was interesting...");

       static_assert(tinyrefl::metadata<mylib::Foo::Enum>().get_value(mylib::Foo::Enum::A).underlying_value() == 0,
           "Great, it seems to work");

       static_assert(tinyrefl::metadata<mylib::Foo::Enum>().get_value(mylib::Foo::Enum::A).name() == "A",
           "Once upon a time there was a proposal to make static_assert() accept constexpr strings as message");

       mylib::Foo foo{"hello", 42};

       tinyrefl::visit_object(foo, [](const std::string& name, auto depth, const auto& member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
       {
           std::cout << "Foo::" << name << ": " << member << "\n";
       });

       using base_classes = typename tinyrefl::metadata<mylib::Foo>::base_classes;
   }
   ```

 - **API agnostic code generation**: The `tinyrefl-tool` codegen tool included in the project is completely independent from the reflection API, generating 
   macro based C/C++ code to allow users to write their own reflection API:

   ``` cpp
   #include <boost/fusion/adapted/struct/adapt_struct.hpp>

   // Write boost-adapt-struct compatible API:
   #define TINYREFL_STRING(...) # __VA_ARGS__
   #define TINYREFL_VALUE(...) __VA_ARGS__
   #define TINYREFL_SEQUENCE(...) __VA_ARGS__
   #define TINYREFL_TYPE(name, fullname) fullname
   #define TINYREFL_MEMBER(name, fullname, type, pointer) ((typename std::result_of<decltype(pointer), type>::type), (pointer))
   #define TINYREFL_REFLECT_CLASS(type, bases, members, classes, enums) BOOST_FUSION_ADAPT_STRUCT(type, members)

   #include "foo.h"
   #include "foo.h.tinyrefl"
   ```

 - **CMake integration**: Both the provided reflection API and codegen tool can be integrated in CMake based projects. To enable reflection,
   the library provides the `tinyrefl_tool()` function:

   ``` cmake

   # Include tinyrefl and its tooling
   add_subdirectory(thirdparty/tinyrefl)
   include(${TINYREFL_SOURCE_DIR}/tool/driver.cmake)

   # Declare your library
   add_library(mylib foo.h foo.cpp)

   # Enable code generation for foo.h of your library
   # The tool will be built and run before building your library
   tinyrefl_tool(TARGET mylib HEADERS foo.h)
   ```

 - **Cross compilation supported**: The CMake setup of tinyrefl detects whether your proyect is being
   cross compiled with a custom CMake toolchain file, and configures itself accordingly. No changes are needed
   in the tool integration as shown in the previous example.
