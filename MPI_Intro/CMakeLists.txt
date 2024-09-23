find_package(MPI REQUIRED)

set(target MPI_Intro_Task1)
add_executable(${target} task1.c)
target_link_libraries(${target} PRIVATE ${MPI_C_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})

set(target MPI_Intro_Task2)
add_executable(${target} task2.c)
target_link_libraries(${target} PRIVATE ${MPI_C_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})

set(target MPI_Intro_Task3)
add_executable(${target} task3.c)
target_link_libraries(${target} PRIVATE ${MPI_C_LIBRARIES})
target_include_directories(${target} PRIVATE ${MPI_INCLUDE_PATH})
