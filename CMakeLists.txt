# CMakeList.txt : CMake project for Project476, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Project476")

file(GLOB helloworld_SRC
    "*.h"
    "*.cpp"
)
# Add source to this project's executable.
add_executable (Project476  ${helloworld_SRC})

# TODO: Add tests and install targets if needed.
