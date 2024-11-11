find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

set(target MPI_Base)
add_executable(${target} mpi_base.cc)
target_link_libraries(${target} PRIVATE ${MPI_CXX_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})

set(target OMP_Base)
add_executable(${target} omp_base.cc)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})

set(target MPI_1c)
add_executable(${target} mpi_1c.cc)
target_link_libraries(${target} PRIVATE ${MPI_CXX_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})

set(target OMP_3b)
add_executable(${target} omp_3b.cc)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})
