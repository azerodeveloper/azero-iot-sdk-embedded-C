set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm64)
set(CROSS_COMPILING_ROOT /usr/local/share/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu)

SET(CMAKE_C_COMPILER    ${CROSS_COMPILING_ROOT}/bin/aarch64-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER  ${CROSS_COMPILING_ROOT}/bin/aarch64-linux-gnu-g++)

if(NOT DEFINED CROSS_COMPILING_ROOT)
  set(CROSS_COMPILING_ROOT $ENV{CROSS_COMPILING_ROOT})
endif()

if(NOT DEFINED CROSS_COMPILING_ROOT)
  message(FATAL_ERROR "Missing CROSS_COMPILING_ROOT")
elseif(NOT IS_DIRECTORY ${CROSS_COMPILING_ROOT})
  message(FATAL_ERROR "CROSS_COMPILING_ROOT is not directory: ${CROSS_COMPILING_ROOT}")
endif()

set(CMAKE_FIND_ROOT_PATH ${CROSS_COMPILING_ROOT}/aarch64-linux-gnu/libc)
message( "CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}" )
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

UNSET(CMAKE_C_FLAGS CACHE)

set(SYSTEM_DETAILS aarch64-Linux)

option( IOT_BUILD_FOR_LINUX_AARCH64  "Build for aarch64."  ON )