#include <fstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <string_view>

#define ISIZE 5000
#define JSIZE 5000

template<typename T>
struct two_dim_arr_omp: private std::vector<std::vector<T>>  {
  using size_type = size_t;
  using data_type = std::vector<std::vector<T>>;
  using data_type::begin;
  using data_type::end;
  using data_type::resize;
  using data_type::operator[];

  two_dim_arr_omp(size_type j1, size_type j2) {
    resize(j1);
    for (auto&& row: *this)
      row.resize(j2);
  }
};

template<typename T>
struct two_dim_arr_mpi {
  using size_type = size_t;
  using data_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  two_dim_arr_mpi(size_type j1, size_type j2)
  : m_j1(j1), m_j2(j2) {
    m_data = new data_type[j1 * j2];
  }

  ~two_dim_arr_mpi() {
    delete[] m_data;
  }

  pointer operator[](size_type n) {
    return m_data + n * m_j2;
  }

  const_pointer operator[](size_type n) const {
    return m_data + n * m_j2;
  }

private:
  size_type m_j1;
  size_type m_j2;
  pointer m_data;
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
