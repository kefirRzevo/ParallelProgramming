find_package(MPI REQUIRED)

set(target MPI_Sends_Task)
add_executable(${target} task.c)
target_link_libraries(${target} PRIVATE ${MPI_C_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})
