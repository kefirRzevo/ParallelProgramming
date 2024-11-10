find_package(MPI REQUIRED)
find_package(GMP REQUIRED)

set(target MPI_Exp_Task)
add_executable(${target} task.cpp)
target_link_libraries(${target} PRIVATE ${MPI_CXX_LIBRARIES})
target_link_libraries(${target} PRIVATE MPI_CXX)
target_link_libraries(${target} gmp gmpxx)
