find_package(MPI REQUIRED)

set(target MPI_Servers_and_files_Task1)
add_executable(${target} task1.cpp)
target_link_libraries(${target} PRIVATE ${MPI_CXX_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})

set(target MPI_Servers_and_files_Task2)
add_executable(${target} task2.cpp)
target_link_libraries(${target} PRIVATE ${MPI_CXX_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})

set(target MPI_Servers_and_files_Task3)
add_executable(${target} task3.cpp)
target_link_libraries(${target} PRIVATE ${MPI_CXX_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})
