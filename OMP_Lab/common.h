#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <string_view>

#define ISIZE 5000
#define JSIZE 5000

template<typename T>
struct two_dim_arr: private std::vector<std::vector<T>>  {
  using size_type = size_t;
  using data_type = std::vector<std::vector<T>>;
  using data_type::begin;
  using data_type::end;
  using data_type::resize;
  using data_type::operator[];

  two_dim_arr(size_type j1, size_type j2) {
    resize(j1);
    for (auto&& row: *this)
      row.resize(j2);
  }
};

template<typename Container>
inline void to_file(std::string_view filename, const Container& data) {
  auto os = std::ofstream{filename.data()};
  for (auto i = int{}; i != ISIZE; ++i) {
    for (auto j = int{}; j != JSIZE; ++j)
      os << std::fixed << data[i][j] << " ";
    os << "\n";
  }
}

inline void report(uint32_t threads, double time_sec) {
  std::cout << "Number of executors: " << threads << std::endl;
  std::cout << "Elapsed time: " << time_sec << " seconds" << std::endl;
}
