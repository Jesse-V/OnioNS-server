cmake_minimum_required(VERSION 2.8)

project(onions-server)

find_package(Boost COMPONENTS system REQUIRED)

#compiler options for Clang and GCC/G++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-exit-time-destructors -Wno-weak-vtables -Wno-padded -Wno-deprecated -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro -Wno-missing-noreturn")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wall -Wextra -pedantic -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations")
endif()

include_directories(/usr/include/onions-common/ /usr/include/botan-1.10)

add_executable(onions-server
  main.cpp

  Mirror.cpp
  Page.cpp
  tcp/Server.cpp
  tcp/Session.cpp
)

FIND_LIBRARY(onions-common NAMES "onions-common" REQUIRED
  PATHS /usr/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-cyoencode NAMES "onions-cyoencode" REQUIRED
  PATHS /usr/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-jsoncpp NAMES "onions-jsoncpp" REQUIRED
  PATHS /usr/lib/onions-common NO_DEFAULT_PATH)
FIND_LIBRARY(onions-scrypt NAMES "libscrypt.so.0" REQUIRED
  PATHS /usr/lib/onions-common NO_DEFAULT_PATH)

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

target_link_libraries(onions-server ${onions-common} ${onions-cyoencode}
  ${onions-jsoncpp} ${onions-scrypt} popt botan-1.10 pthread
  ${Boost_SYSTEM_LIBRARY} ${Boost_LIBRARIES})

#installation of files
set(DEB_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/assets")
install(TARGETS onions-server                   DESTINATION /usr/bin/)
install(FILES ${DEB_FOLDER}/onions-server.1.gz  DESTINATION /usr/share/man/man1)

