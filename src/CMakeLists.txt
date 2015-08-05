cmake_minimum_required(VERSION 2.8)

project(onions-server)

find_package(Boost COMPONENTS system REQUIRED)

#compiler options for Clang and GCC/G++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wno-c++98-compat-pedantic -pedantic -Weverything -Wno-exit-time-destructors -Wno-weak-vtables -Wno-padded -Wno-deprecated -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-g -O3 --std=c++11 -Wall -Wextra -pedantic -Wdouble-promotion -Wfloat-equal -Wunsafe-loop-optimizations")
endif()

include_directories(/usr/include/onions-common/include /usr/include/botan-1.10)

add_executable(onions-server
  main.cpp

  Mirror.cpp
  tcp/Server.cpp
  tcp/Session.cpp
)

target_link_libraries(onions-server onions-common onions-jsoncpp onions-cyoencode popt botan-1.10 pthread ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_LIBRARIES})

#installation of files
set(DEB_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/assets")
install(TARGETS onions-server                   DESTINATION /usr/bin/)
install(FILES ${DEB_FOLDER}/onions-server.1.gz  DESTINATION /usr/share/man/man1)

