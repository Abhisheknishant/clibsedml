cmake_minimum_required(VERSION 3.5)

project(CLIBSEDML
  VERSION 0.1.3
  LANGUAGES C)

include(CheckIncludeFile)

# check required headers
check_include_file(stdlib.h HAVE_STDLIB_H)
if(NOT HAVE_STDLIB_H)
  message(FATAL_ERROR "stdlib.h is required")
endif(NOT HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)
if(NOT HAVE_STRING_H)
  message(FATAL_ERROR "string.h is required")
endif(NOT HAVE_STRING_H)

include(CheckTypeSize)

include(FindThreads)

find_package(LibXml2 REQUIRED)
include_directories("${LIBXML2_INCLUDE_DIR}")

include_directories("${CLIBSEDML_SOURCE_DIR}/src")

if(CMAKE_COMPILER_IS_GNUCC)
# http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# http://www.gnu.org/software/gsl/manual/html_node/GCC-warning-options-for-numerical-programs.html
add_definitions("-Wall -Wextra -Werror -Wundef -Wconversion -Wmissing-prototypes -Wstrict-prototypes -Wnested-externs -Wpointer-arith -Wshadow -Wcast-align -Wcast-qual -Wwrite-strings")
endif(CMAKE_COMPILER_IS_GNUCC)

add_subdirectory(src)

if(ENABLE_TESTS)
  enable_testing()
  add_subdirectory(test)
endif(ENABLE_TESTS)

set(VERSION "${CLIBSEDML_VERSION_MAJOR}.${CLIBSEDML_VERSION_MINOR}.${CLIBSEDML_VERSION_PATCH}")
configure_file(clibsedml.spec.in "${CLIBSEDML_BINARY_DIR}/clibsedml.spec"
  @ONLY
  )

set(CPACK_PACKAGE_NAME clibsedml)
set(CPACK_PACKAGE_VERSION_MAJOR ${CLIBSEDML_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CLIBSEDML_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CLIBSEDML_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "Takeshi Abe")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C library for SED-ML")
set(CPACK_RESOURCE_FILE_LICENSE "${CLIBSEDML_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CLIBSEDML_SOURCE_DIR}/README")
include(CPack)

option(BUILD_SHARED_LIBS "build clibsedml as a shared library" ON)
option(ENABLE_TESTS "enable and build tests" ON)
