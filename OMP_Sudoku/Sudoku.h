#pragma once

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <string>
#include <utility>
#include <vector>

namespace sudoku {

void failWithError(std::string Msg);

class sudoku_solver {
  using value_type = short;

public:
  struct cell_type {
    value_type m_value = 0;
    std::bitset<16 * sizeof(value_type)> m_possible_values = {};
  };

  struct board_type : private std::vector<std::vector<cell_type>> {
    using vector_type = std::vector<std::vector<cell_type>>;
    using vector_type::at;
    using vector_type::begin;
    using vector_type::data;
    using vector_type::empty;
    using vector_type::end;
    using vector_type::resize;
    using vector_type::size;

    sudoku_solver::value_type m_row;
    sudoku_solver::value_type m_col;
  };

private:
  value_type m_size;
  value_type m_subsize;
  board_type m_original;
  board_type m_solved;
  bool m_solution_found;

public:
  void read(std::istream &is);

  void write_original(std::ostream &os) const;
  void write_solved(std::ostream &os) const;
  bool solve();
  bool is_solved(const board_type &Grid) const;

private:
  void write(std::ostream &os, const board_type &board) const;
  bool isCorrectInput(const board_type &board) const;
  void setValue(board_type &board, value_type row, value_type col,
                value_type value) const;
  void setPossibleValues(board_type &board, value_type row,
                         value_type col) const;
  bool reducePossibleValues(board_type &board, value_type row,
                            value_type col) const;

  // Humanistic alghorithm
  bool solveHumanistic(board_type &board) const;
  bool eliminate(board_type &board) const;
  bool setLoneRangersRow(board_type &board) const;
  bool setLoneRangersColumn(board_type &board) const;
  bool setLoneRangersLittleSquare(board_type &board) const;
  bool setTwinsRow(board_type &board) const;
  bool setTwinsColumn(board_type &board) const;

  // Brute Force alghorithm
  bool fillPermutationStack(board_type &board);
  bool solveBruteForce(board_type &board);
  std::pair<int, int> getLeastUnsureCell(const board_type &board) const;
  void pushIdxPermutations(const std::pair<int, int> &Idx, board_type board,
                           std::vector<board_type> *Stack) const;
};

} // namespace sudoku
