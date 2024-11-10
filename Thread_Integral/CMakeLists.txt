set(target Thread_Integral_Task)
add_executable(${target} task.cpp)
target_include_directories (${target} PRIVATE includes)
