cmake_minimum_required(VERSION 3.0)

project(neural-network)

add_executable(neural-network source/main.cpp)

install(TARGETS neural-network RUNTIME DESTINATION bin)
