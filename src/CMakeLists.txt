cmake_minimum_required(VERSION 2.8)

project(tor-onions)

find_package(Boost COMPONENTS system REQUIRED)

#compiler options for Clang and GCC/G++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-g -O3 --std=c++0x -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-unused-parameter -Wno-global-constructors -Wno-exit-time-destructors -Wno-non-virtual-dtor -Wno-weak-vtables -Wno-padded -Wno-cast-align -Wno-gnu -Wno-nested-anon-types -Wno-documentation -Wno-documentation-unknown-command -Wno-unknown-pragmas")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-g -O3 --std=c++0x -Wall -Wextra -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations -Wno-unused-parameter")
endif()

#JSONCPP needs special compile flags
#https://stackoverflow.com/questions/13638408/cmake-override-compile-flags-for-single-files
SET (JSONCPP_CPP libs/jsoncpp/jsoncpp.cpp)
set_source_files_properties(${JSONCPP_CPP} PROPERTIES COMPILE_FLAGS -w)

#include headers to satisfy dependencies
#libs/CyoEncode libs/jsoncpp
include_directories(libs libs/jsoncpp /usr/include/botan-1.10)

#libscrypt needs to be compiled into .so
add_custom_target(libscrypt
   COMMAND make
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/libscrypt/
)

add_executable(tor-onions
    main.cpp
    Flags.cpp

    client/ClientProtocols.cpp

    client/tcp/SocksClient.cpp
    client/tcp/SocksRequest.cpp
    client/tcp/SocksReply.cpp

    server/ServerProtocols.cpp
    server/Snapshot.cpp
    server/tcp/Server.cpp
    server/tcp/Session.cpp

    hs/HSProtocols.cpp

    common/CommonProtocols.cpp
    common/Environment.cpp
    common/Page.cpp
    common/Quorum.cpp
    common/utils.cpp

    common/records/Record.cpp
    common/records/CreateR.cpp
    common/records/RenewR.cpp
    common/records/MoveR.cpp
    common/records/UpdateR.cpp
    common/records/DeleteR.cpp

    ${JSONCPP_CPP}

    libs/CyoEncode/CyoDecode.c
    libs/CyoEncode/CyoEncode.c
)

#link against built and existing dependencies
add_dependencies(tor-onions libscrypt)
SET (LIBSCRYPT_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/libscrypt/libscrypt.so.0)
target_link_libraries(tor-onions pthread ${LIBSCRYPT_LIB} botan-1.10 ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY} ${Boost_LIBRARIES})

#custom handling of output debug binary
set_target_properties(tor-onions PROPERTIES DEBUG_POSTFIX "_d")

#installation of files
set(DEB_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/debian/extra_includes")
install(TARGETS tor-onions                   DESTINATION /usr/bin/)
install(FILES ${DEB_FOLDER}/tor-client       DESTINATION /usr/bin/)
install(FILES ${LIBSCRYPT_LIB}               DESTINATION /usr/lib/)
install(FILES ${DEB_FOLDER}/tor-onions.1.gz  DESTINATION /usr/share/man/man1)

set(ASSETS "${DEB_FOLDER}/assets")
install(FILES ${ASSETS}/example.key         DESTINATION /var/lib/tor-onions/)
install(FILES ${ASSETS}/cache.txt           DESTINATION /var/lib/tor-onions/)
install(FILES ${ASSETS}/cached-certs        DESTINATION /var/lib/tor-onions/)
install(FILES ${ASSETS}/cached-microdesc-consensus  DESTINATION /var/lib/tor-onions/)
install(FILES ${ASSETS}/cached-microdescs.new       DESTINATION /var/lib/tor-onions/)
