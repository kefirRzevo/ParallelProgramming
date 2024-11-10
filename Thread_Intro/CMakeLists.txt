set(target Thread_Intro_Task1)
add_executable(${target} task1.cpp)
target_include_directories (${target} PRIVATE includes)

set(target Thread_Intro_Task2)
add_executable(${target} task2.cpp)
target_include_directories (${target} PRIVATE includes)

set(target Thread_Intro_Task3)
add_executable(${target} task3.cpp)
target_include_directories (${target} PRIVATE includes)
