#.rst:
# CheckCXXConcepts
# ----------------
#
# Check if the C++ compiler supports constraints for template arguments (i.e.
# `concepts <http://en.wikipedia.org/wiki/Concepts_%28C%2B%2B%29>`_).
#
# ::
#
#   check_cxx_concepts(<variable>)
#
# ``<variable>``
#   The variable that will store the result.
#
# The following variables may be set before calling this macro to modify the
# way the check is run:
#
# ``CMAKE_REQUIRED_FLAGS``
#   String of compile command line flags.
# ``CMAKE_REQUIRED_DEFINITIONS``
#   List of macros to define (-DFOO=bar).
# ``CMAKE_REQUIRED_INCLUDES``
#   List of include directories.
# ``CMAKE_REQUIRED_LIBRARIES``
#   List of libraries to link.
# ``CMAKE_REQUIRED_QUIET``
#   Execute quietly without messages.
#
# .. note::
#    As of 2015, the only known compiler that supports this language feature
#    is `GCC 6.0 <https://gcc.gnu.org>`_.
#
# Requires CMake 2.8 or later.

include(CheckCXXSourceCompiles)

macro(CHECK_CXX_CONCEPTS VARIABLE)
  check_cxx_source_compiles("
template<typename T>
concept bool
Equality_comparable() {
  return requires (T a, T b) {
    { a == b } -> bool;
    { a != b } -> bool;
  };
}

int main() {
  return 0;
}
    " ${VARIABLE})
endmacro()
