cmake_minimum_required(VERSION 2.8)

project(onions-server)

# define compiler options for Clang and GCC/G++
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -fstack-protector -D_FORTIFY_SOURCE=1")
SET(CLANG_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-exit-time-destructors -Wno-weak-vtables -Wno-padded -Wno-deprecated -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-missing-noreturn -Wno-sign-conversion -Wno-shadow")
SET(GCC_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS_DEBUG "${CLANG_DEBUG}")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG "${GCC_DEBUG}")
endif()

include_directories(${CMAKE_INSTALL_PREFIX}/include/onions-common /usr/include/botan-1.10)

add_executable(onions-server
  main.cpp
  Server.cpp
  containers/Cache.cpp
  containers/MerkleTree.cpp
)

FIND_LIBRARY(botan NAMES "botan-1.10" REQUIRED)
FIND_LIBRARY(argtable2 NAMES "argtable2" REQUIRED)

# find main onions-common library components
FIND_LIBRARY(onions-common NAMES "onions-common" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-cyoencode NAMES "onions-cyoencode" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsoncpp NAMES "onions-jsoncpp" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)

# find json-rpc-cpp components
FIND_LIBRARY(onions-jsonrpccpp-common NAMES "jsonrpccpp-common" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsonrpccpp-client NAMES "jsonrpccpp-client" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsonrpccpp-server NAMES "jsonrpccpp-server" REQUIRED
  PATHS ${CMAKE_INSTALL_PREFIX}/lib/onions-common NO_DEFAULT_PATH)

if (${botan} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "Botan could not be found! Please install botan-devel or botan-dev.")
else()
  message(STATUS "Found Botan library at ${botan}")
endif()

if (${argtable2} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "Argtable2 could not be found! Please install libargtable2-dev")
else()
  message(STATUS "Found Argtable2 library at ${argtable2}")
endif()

if (${onions-common} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common could not be found! Please install tor-onions-common.")
else()
  message(STATUS "Found OnioNS-common library at ${onions-common}")
endif()

if (${onions-cyoencode} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's CyoEncode component was not found!")
else()
  message(STATUS "Found OnioNS-common's CyoEncode component at ${onions-cyoencode}")
endif()

if (${onions-jsoncpp} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's jsoncpp component was not found!")
else()
  message(STATUS "Found OnioNS-common's JsonCpp component at ${onions-jsoncpp}")
endif()

if (${onions-jsonrpccpp-server} MATCHES ".*-NOTFOUND")
  message(FATAL_ERROR "OnioNS-common's JSON-RPC-CPP component was not found!")
else()
  message(STATUS "Found JSON-RPC-CPP's Server component at ${onions-jsonrpccpp-server}")
endif()

target_link_libraries(onions-server ${onions-common} ${onions-jsoncpp}
  ${onions-jsonrpccpp-common} ${onions-jsonrpccpp-server} ${onions-cyoencode}
  ${argtable2} ${botan} pthread)

#installation of files
set(DEB_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/assets")
install(TARGETS onions-server                   DESTINATION bin)
install(FILES ${DEB_FOLDER}/onions-server.1.gz  DESTINATION share/man/man1)
