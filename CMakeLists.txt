cmake_minimum_required(VERSION 3.0)

project(neural-network)

add_executable(neural-network
    source/snn.cpp
    source/sclt.cpp
    source/app.cpp
    source/sts.cpp
    source/main.cpp
)

add_executable(mnist-test
    source/snn.cpp
    source/sclt.cpp
    source/mnist.cpp
)

install(TARGETS neural-network RUNTIME DESTINATION bin)
