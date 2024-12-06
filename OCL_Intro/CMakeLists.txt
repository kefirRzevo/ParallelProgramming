#cmake_minimum_required(VERSION 3.16)
#project(OCL_Devices)
#
#find_package(OpenCL REQUIRED)
#include_directories(include)
#
#if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    #set (COMPILER_SETTINGS -g -Wall -Wextra -Wpedantic)
#else ()
    #set (COMPILER_SETTINGS -g -Wall -Wextra -Wpedantic)
#endif ()
#add_compile_options(${COMPILER_SETTINGS})
#
#set (SOURCES devices.cc)
#
#add_executable (${PROJECT_NAME} ${SOURCES})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_libs} "-framework OpenCL")
