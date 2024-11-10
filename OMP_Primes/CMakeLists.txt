cmake_minimum_required(VERSION 3.29)
project(primes)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(OpenMP REQUIRED COMPONENTS CXX)

function(easy_add_executable target_name)
  add_executable(${target_name} Primes.cc)
  target_compile_features(${target_name} PUBLIC cxx_std_20)
  target_link_libraries(${target_name} PRIVATE OpenMP::OpenMP_CXX
                                               Boost::program_options)
endfunction()

easy_add_executable(main)
