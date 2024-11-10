find_package(MPI REQUIRED)

set(target MPI_Lab_Task)
add_executable(${target} task.cpp)
target_link_libraries(${target} PRIVATE ${MPI_CXX_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})
