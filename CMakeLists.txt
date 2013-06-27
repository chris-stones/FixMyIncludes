cmake_minimum_required(VERSION 2.8)

project(fixmyincludes)

add_executable(fixmyincludes main.cpp config.cpp)

target_link_libraries (fixmyincludes boost_filesystem boost_system imgutil)

install(TARGETS fixmyincludes RUNTIME DESTINATION bin)
