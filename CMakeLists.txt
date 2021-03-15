cmake_minimum_required(VERSION 3.1)
project(ScalaComputing)

add_subdirectory(third-party/yaml-cpp) # This is one way of referencing another
                                       # CMake project

add_executable(
    Scala-Computing
    main.cpp
)

target_link_libraries(
    Scala-Computing  
    PRIVATE
    yaml-cpp  # This is the "target" exposed by 
              # yaml-cpp's CMakeLists.txt file
)
