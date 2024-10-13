find_package(OpenMP REQUIRED)

set(target OMP_Tasks_Balance)
add_executable(${target} balance.cpp)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})
target_compile_definitions(${target} PRIVATE SCHEDULE_DYNAMIC)

set(target OMP_Tasks_Nested)
add_executable(${target} nested.cpp)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})
target_compile_definitions(${target} PRIVATE SCHEDULE_DYNAMIC)

set(target OMP_Tasks_Untied)
add_executable(${target} untied.cpp)
target_compile_options(${target} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${target} PRIVATE ${OpenMP_CXX_FLAGS})
target_compile_definitions(${target} PRIVATE SCHEDULE_DYNAMIC)
