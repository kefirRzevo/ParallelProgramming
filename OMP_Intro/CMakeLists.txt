find_package(OpenMP REQUIRED)

set(target OMP_Intro_Task1)
add_executable(${target} task1.cpp)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})

set(target OMP_Intro_Task2)
add_executable(${target} task2.cpp)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})

set(target OMP_Intro_Task3)
add_executable(${target} task3.cpp)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})
