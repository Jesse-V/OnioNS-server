cmake_minimum_required(VERSION 2.8)

project(onions-server)

#compiler options for Clang and GCC/G++
SET(COMMON_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -DED25519_CUSTOMHASH -DED25519_CUSTOMRANDOM -fstack-protector -D_FORTIFY_SOURCE=1")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-exit-time-destructors -Wno-weak-vtables -Wno-padded -Wno-deprecated -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-missing-noreturn")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -Wall -Wextra -pedantic -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations")
endif()

set(ONIONS_INCLUDE ${CMAKE_INSTALL_PREFIX}/include/onions-common/)
include_directories(${ONIONS_INCLUDE} /usr/include/botan-1.10)

add_executable(onions-server
  main.cpp
  Server.cpp
  Page.cpp
)

FIND_LIBRARY(onions-common NAMES "onions-common" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-cyoencode NAMES "onions-cyoencode" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsoncpp NAMES "onions-jsoncpp" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-scrypt NAMES "libscrypt.so.0" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(jsonrpccpp-common NAMES "libjsonrpccpp-common.so" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(jsonrpccpp-client NAMES "libjsonrpccpp-client.so" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(jsonrpccpp-server NAMES "libjsonrpccpp-server.so" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)

if (${onions-common} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common could not be found! Please install it.")
else()
  message(STATUS "Found OnioNS-common library")
endif()

if (${onions-cyoencode} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's CyoEncode component was not found!")
else()
  message(STATUS "Found OnioNS-common's CyoEncode component")
endif()

if (${onions-jsoncpp} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's jsoncpp component was not found!")
else()
  message(STATUS "Found OnioNS-common's JsonCpp component")
endif()

if (${onions-scrypt} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's scrypt component was not found!")
else()
  message(STATUS "Found OnioNS-common's libscrypt component")
endif()

if (${jsonrpccpp-common} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's JSON-RPC-CPP's Common component was not found!")
else()
  message(STATUS "Found JSON-RPC-CPP's Common component")
endif()

if (${jsonrpccpp-client} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's JSON-RPC-CPP's Client component was not found!")
else()
  message(STATUS "Found JSON-RPC-CPP's Client component")
endif()

if (${jsonrpccpp-server} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's JSON-RPC-CPP's Server component was not found!")
else()
  message(STATUS "Found JSON-RPC-CPP's Server component")
endif()

target_link_libraries(onions-server ${jsonrpccpp-common} ${jsonrpccpp-client} ${jsonrpccpp-server} ${onions-common} ${onions-cyoencode} ${onions-jsoncpp} ${onions-scrypt} argtable2 botan-1.10 pthread)

#installation of files
set(DEB_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/assets")
install(TARGETS onions-server                   DESTINATION bin)
install(FILES ${DEB_FOLDER}/onions-server.1.gz  DESTINATION share/man/man1)

