cmake_minimum_required(VERSION 3.16)
project(OCL_Devices)

find_package(OpenCL REQUIRED)

set (SOURCES devices.cc)
add_executable (${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_libs} "-framework OpenCL")
