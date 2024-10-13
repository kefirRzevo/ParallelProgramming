find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(target OMP_Sudoku)
add_executable(${target} sudoku.cpp)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})
target_compile_definitions(${target} PRIVATE SCHEDULE_DYNAMIC)
