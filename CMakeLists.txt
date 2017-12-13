cmake_minimum_required (VERSION 3.2.2)
project ("VPA: Variable Precision Arithmetic Library - Native Version")
set (VPA_VERSION 0.2)

set(CMAKE_CXX_STANDARD 11)

# Generate the library
add_library(vpa_n ${CMAKE_SOURCE_DIR}/src/vpa_n.cpp ${CMAKE_SOURCE_DIR}/inc/vpa_n.h)

# Include directories
target_include_directories(vpa_n
                           PRIVATE ${CMAKE_SOURCE_DIR}/inc
                          )
include_directories(${CMAKE_SOURCE_DIR}/inc)


# Compiler options
target_compile_options(vpa_n
                      PRIVATE -m64 -lc++
                      )

# Generate the test add_executable
add_executable(vpaConverter
               ${CMAKE_SOURCE_DIR}/main.cpp
              )
target_include_directories(vpaConverter PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(vpaConverter vpa_n)

add_custom_target(runTest
    COMMAND vpaConverter -m t
    DEPENDS vpaConverter
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
