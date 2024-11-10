cmake_minimum_required (VERSION 3.16)

set(CMAKE_CXX_COMPILER "clang++-18")
set(CMAKE_CXX_STANDARD 17)

project(paralle_programming)

# Problem with GMP and clang++-18 - do it manually
# add_subdirectory(MPI_Exp)
add_subdirectory(MPI_Intro)
add_subdirectory(MPI_Lab)
add_subdirectory(MPI_Sends)
add_subdirectory(MPI_Servers_and_files)
add_subdirectory(MPI_Sort)
add_subdirectory(OMP_Intro)
add_subdirectory(OMP_Tasks)
add_subdirectory(OMP_Primes)
add_subdirectory(OMP_Sudoku)
add_subdirectory(Thread_Integral)
add_subdirectory(Thread_Intro)
