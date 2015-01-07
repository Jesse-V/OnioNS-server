cmake_minimum_required(VERSION 2.6)

project(EsgalDNS)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-unused-parameter -Wno-global-constructors -Wno-exit-time-destructors -Wno-non-virtual-dtor -Wno-weak-vtables -Wno-padded -Wno-cast-align -Wno-gnu -Wno-nested-anon-types -Wno-documentation-unknown-command -Wno-unknown-pragmas")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wall -Wextra -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations -Wno-unused-parameter")
endif()

include_directories(.)

#organized by importance
add_executable(EsgalDNS
    main.cpp
    Records/Domain.cpp
    Records/Record.cpp
    utils.cpp
    libs/base64.cpp
)

#libssl-dev is a dependency

SET (LIBSCRYPT_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/libscrypt-1.20/libscrypt.so)
target_link_libraries(EsgalDNS pthread ${LIBSCRYPT_LIB} crypto)
