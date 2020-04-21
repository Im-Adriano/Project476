
cmake_minimum_required (VERSION 3.8)
set(CMAKE_BUILD_TYPE release)
set(CMAKE_CXX_FLAGS_DEBUG "/g")
set(CMAKE_CXX_FLAGS_RELEASE "/O3")
project ("Project476")

file(GLOB helloworld_SRC
    "*.h"
    "*.cpp"
)
add_executable (Project476  ${helloworld_SRC})
add_library(_Project476 SHARED ${helloworld_SRC} )

