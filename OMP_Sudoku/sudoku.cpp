#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <omp.h>
#include <sstream>
#include <stdexcept>
#include <vector>

#define rep std::cout << "at " << __LINE__ << std::endl

class Sudoku {
  struct Cell;

  using value_type = size_t;
  using input_iter = std::istream_iterator<value_type>;
  using data_type = std::vector<std::vector<Cell>>;

  struct Cell {
    value_type m_value = 0;
    bool m_fixed = false;
  };

  value_type m_size;
  value_type m_subsize;
  std::vector<std::vector<Cell>> m_data;

  bool validate(const data_type &data, value_type size) const {
    return std::all_of(data.cbegin(), data.cend(), [size](auto &&row) {
      return std::all_of(row.cbegin(), row.cend(), [size](auto &&elem) {
        return elem.m_value >= 0 && elem.m_value <= size;
      });
    });
  }

  bool is_possible(value_type row, value_type col, value_type value) const {
    bool possible = true;
#pragma omp parallel for shared(possible)
    for (value_type i = 0; i != m_size; ++i) {
      if (m_data[row][i].m_value == value && m_data[row][i].m_fixed)
        possible = false;
    }
    if (!possible)
      return false;

#pragma omp parallel for shared(possible)
    for (value_type i = 0; i != m_size; ++i) {
      if (m_data[i][col].m_value == value && m_data[i][col].m_fixed)
        possible = false;
    }
    if (!possible)
      return false;

    auto start_row = row - row % m_subsize;
    auto start_col = col - col % m_subsize;
#pragma omp parallel for collapse(2) shared(possible)
    for (value_type i = 0; i != m_subsize; ++i) {
      for (value_type j = 0; j != m_subsize; ++j) {
        if (m_data[i + start_row][j + start_col].m_value == value &&
            m_data[i + start_row][j + start_col].m_fixed)
          possible = false;
      }
    }

    return possible;
  }

  bool solve(value_type row, value_type col) {
    if (row == m_size - 1 && col == m_size)
      return true;

    if (col == m_size) {
      row++;
      col = 0;
    }

    if (m_data[row][col].m_fixed)
      return solve(row, col + 1);

    for (value_type value = 1; value <= m_size; ++value) {
      if (is_possible(row, col, value)) {
        m_data[row][col].m_value = value;
        m_data[row][col].m_fixed = true;
        if (solve(row, col + 1))
          return true;
        m_data[row][col].m_value = 0;
        m_data[row][col].m_fixed = false;
      }
    }
    return false;
  }

public:
  void read(std::istream &is) {
    value_type size = 0;
    value_type subsize = 0;
    value_type value = 0;
    data_type data = {};
    is >> subsize;
    size = subsize * subsize;
    data.resize(size);
    for (value_type i = 0; i != size; ++i) {
      data[i].resize(size);
      for (value_type j = 0; j != size; ++j) {
        if (value != 0) {
          data[i][j].m_value = value;
          data[i][j].m_fixed = true;
        }
      }
    }
    if (!validate(data, size))
      throw std::runtime_error("incorrect data set");
    m_size = size;
    m_subsize = subsize;
    m_data = std::move(data);
  }

  void write(std::ostream &os) const {
    os << m_subsize << "\n";
    for (value_type i = 0; i != m_size; ++i) {
      for (value_type j = 0; j != m_size; ++j) {
        os << m_data[i][j].m_value << " ";
      }
      os << "\n";
    }
  }

  bool solve() { return solve(0, 0); }
};

auto main(int argc, const char *argv[]) -> int {
  try {
    if (argc != 3)
      throw std::runtime_error(
          "usage: <1> num threads <2> path to file with sudoku");

    auto num_threads = std::atoi(argv[1]);
    omp_set_dynamic(0);
    omp_set_num_threads(num_threads);
    std::ifstream is{argv[2]};
    Sudoku s;
    s.read(is);
    if (s.solve()) {
      s.write(std::cout);
      return 0;
    } else {
      std::cout << "No solutions were found" << std::endl;
      return 1;
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
