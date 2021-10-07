cmake_minimum_required(VERSION 3.10.2)
project(helloworld VERSION 1.0.0 LANGUAGES C CXX)

# Get the sources and add them
file(GLOB HELLOWORLD_SOURCES src/*.c src/*.h)

add_executable(helloworld ${HELLOWORLD_SOURCES})
set_property(TARGET helloworld PROPERTY CXX_STANDARD 17)

#target_link_libraries(helloworld pthread)
