cmake_minimum_required (VERSION 3.3)
project ("VPA: Variable Precision Arithmetic Library")
set (VPA_VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)

# Generate the library
add_library(vpa ${CMAKE_SOURCE_DIR}/src/vpa.cpp ${CMAKE_SOURCE_DIR}/inc/vpa.h)

# Include directories
target_include_directories(vpa
                           PRIVATE ${CMAKE_SOURCE_DIR}/inc
                          )
include_directories(${CMAKE_SOURCE_DIR}/inc)


# Compiler options
target_compile_options(vpa
                      PRIVATE -m64 -lc++
                      )

# Generate the test add_executable
add_executable(vpaConverter
               ${CMAKE_SOURCE_DIR}/src/main.cpp
              )
target_include_directories(vpaConverter PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(vpaConverter vpa)

add_custom_target(runTest
    COMMAND vpaConverter -m t
    DEPENDS vpaConverter
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
