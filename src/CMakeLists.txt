cmake_minimum_required(VERSION 2.8)

project(OnioNS)

#compiler options for Clang and GCC/G++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-unused-parameter -Wno-global-constructors -Wno-exit-time-destructors -Wno-non-virtual-dtor -Wno-weak-vtables -Wno-padded -Wno-cast-align -Wno-gnu -Wno-nested-anon-types -Wno-documentation-unknown-command -Wno-unknown-pragmas")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wall -Wextra -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations -Wno-unused-parameter")
endif()

#JSONCPP needs special compile flags
#https://stackoverflow.com/questions/13638408/cmake-override-compile-flags-for-single-files
SET (JSONCPP_CPP libs/jsoncpp-1.1.0/jsoncpp.cpp)
set_source_files_properties(${JSONCPP_CPP} PROPERTIES COMPILE_FLAGS -w)

#include headers to satisfy dependencies
include_directories(. libs/jsoncpp-1.1.0 /usr/include/botan-1.10 libs/CyoEncode-1.1.1)

#libscrypt needs to be compiled into .so
add_custom_target(libscrypt
   COMMAND make
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/libscrypt-1.20/
)

add_executable(OnioNS
    main.cpp

    Database/Page.cpp
    Database/Snapshot.cpp

    Records/Record.cpp
    Records/Registration.cpp
    Records/Renewal.cpp
    Records/Update.cpp
    Records/Transfer.cpp
    Records/Deletion.cpp

    utils.cpp
    ${JSONCPP_CPP}

    libs/CyoEncode-1.1.1/CyoDecode.c
    libs/CyoEncode-1.1.1/CyoEncode.c
)

#link against built and existing dependencies
add_dependencies(OnioNS libscrypt)
SET (LIBSCRYPT_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/libscrypt-1.20/libscrypt.so.0)
target_link_libraries(OnioNS pthread ${LIBSCRYPT_LIB} botan-1.10)

#custom handling of output binary
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
message("CMAKE SRC DIR is ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE BUILD DIR is ${CMAKE_CURRENT_BUILD_DIR}")
set_target_properties(OnioNS PROPERTIES DEBUG_POSTFIX "_d")

#installation of files
set(DEB_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/debian/extra_includes")
install(TARGETS OnioNS                  DESTINATION /usr/bin/)
install(FILES ${LIBSCRYPT_LIB}           DESTINATION /usr/lib/)
install(FILES ${DEB_FOLDER}/OnioNS.1.gz  DESTINATION /usr/share/man/man1)
