find_package(Boost REQUIRED COMPONENTS program_options)
find_package(OpenMP REQUIRED COMPONENTS CXX)
set(CMAKE_CXX_FLAGS "-O3")

set(target OMP_Equation)
add_executable(${target} equation.cc)
target_compile_features(${target} PUBLIC cxx_std_20)
target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX Boost::program_options)
