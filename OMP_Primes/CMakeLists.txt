find_package(Boost REQUIRED COMPONENTS program_options)
find_package(OpenMP REQUIRED COMPONENTS CXX)

set(target OMP_Primes)
add_executable(${target} primes.cc)
target_compile_features(${target} PUBLIC cxx_std_20)
target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX Boost::program_options)
