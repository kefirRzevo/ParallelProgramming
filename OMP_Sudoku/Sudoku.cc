#include "Sudoku.h"

namespace sudoku {

constexpr size_t stack_size_g = 20;

std::vector<sudoku_solver::board_type> permutations_stack_g;

void sudoku_solver::read(std::istream &is) {
  sudoku_solver solver;

  is >> solver.m_subsize;
  solver.m_size = solver.m_subsize * solver.m_subsize;
  solver.m_solution_found = false;

  board_type original;

  original.resize(solver.m_size);
  for (auto &row : original) {
    row.resize(solver.m_size);
    for (auto &cell : row) {
      value_type value = 0;
      is >> value;
      cell.m_value = value;
    }
  }

  if (!solver.isCorrectInput(original))
    throw std::logic_error("Incorrect start grid");
  solver.m_original = original;
  *this = solver;
}

bool sudoku_solver::isCorrectInput(const board_type &Brd) const {
  std::vector<bool> ValuesBeen(m_size, false);
  auto clearValuesBeen = [&]() {
    ValuesBeen.clear();
    ValuesBeen.resize(m_size, false);
  };

  auto checkRowCol = [&](auto Row, auto Col) {
    auto Value = Brd.at(Row).at(Col).m_value;
    if (Value == 0)
      return true;
    if (ValuesBeen[Value - 1] == true)
      return false;
    ValuesBeen[Value - 1] = true;
    return true;
  };

  // Check rows
  for (auto Row = 0; Row < m_size; ++Row) {
    for (auto Col = 0; Col < m_size; ++Col)
      if (!checkRowCol(Row, Col))
        return false;
    clearValuesBeen();
  }

  // Check columns
  for (auto Col = 0; Col < m_size; ++Col) {
    for (auto Row = 0; Row < m_size; ++Row)
      if (!checkRowCol(Row, Col))
        return false;
    clearValuesBeen();
  }

  if (!ValuesBeen.empty())
    ValuesBeen.clear();
  ValuesBeen.resize(m_size, false);
  // Check little squares
  for (auto SqRow = 0; SqRow < m_size; SqRow = SqRow + m_subsize) {
    for (auto SqCol = 0; SqCol < m_size; SqCol = SqCol + m_subsize) {
      for (auto Row = SqRow; Row < SqRow + m_subsize; ++Row) {
        for (auto Col = SqCol; Col < SqCol + m_subsize; ++Col)
          if (!checkRowCol(Row, Col))
            return false;
      }
      clearValuesBeen();
    }
  }
  return true;
}

void sudoku_solver::setValue(board_type &board, value_type row, value_type col,
                             value_type value) const {
  board.at(row).at(col).m_value = value;
}

bool sudoku_solver::is_solved(const board_type &Grid) const {
  std::vector<bool> ValuesBeen(m_size, false);

  auto clearValuesBeen = [&]() {
    ValuesBeen.clear();
    ValuesBeen.resize(m_size, false);
  };

  auto checkLess = [&](auto Row, auto Col) {
    auto Value = Grid.at(Row).at(Col).m_value;
    if (ValuesBeen[Value - 1] == true)
      throw std::logic_error("Repeat value " + std::to_string(Value) + " on " +
                             std::to_string(Row) +
                             " row, col or little square");
    ValuesBeen[Value - 1] = true;
    return true;
  };
  auto checkRowCol = [&](auto Row, auto Col) {
    auto Value = Grid.at(Row).at(Col).m_value;
    if (Value == 0)
      return /* Cell unfilled */ false;
    if (m_original.at(Row).at(Col).m_value &&
        (Value != m_original.at(Row).at(Col).m_value))
      throw std::logic_error("Value in (" + std::to_string(Row) + ", " +
                             std::to_string(Col) +
                             ") cell differ from original matrix");
    return checkLess(Row, Col);
  };

  // Check rows
  for (auto Row = 0; Row < m_size; ++Row) {
    for (auto Col = 0; Col < m_size; ++Col)
      if (!checkRowCol(Row, Col))
        return false;
    clearValuesBeen();
  }

  // Check columns
  for (auto Col = 0; Col < m_size; ++Col) {
    for (auto Row = 0; Row < m_size; ++Row)
      if (!checkLess(Row, Col))
        return false;
    clearValuesBeen();
  }

  // Check little squares
  for (auto SqRow = 0; SqRow < m_size; SqRow = SqRow + m_subsize) {
    for (auto SqCol = 0; SqCol < m_size; SqCol = SqCol + m_subsize) {
      for (auto Row = SqRow; Row < SqRow + m_subsize; ++Row) {
        for (auto Col = SqCol; Col < SqCol + m_subsize; ++Col)
          if (!checkLess(Row, Col))
            return false;
      }
      clearValuesBeen();
    }
  }
  return true;
}

void sudoku_solver::write(std::ostream &os, const board_type &Brd) const {
  for (auto Row = 0; Row < m_size; ++Row) {
    for (auto Col = 0; Col < m_size; ++Col) {
      os << Brd.at(Row).at(Col).m_value;
      if (Brd.at(Row).at(Col).m_value / 10u == 0)
        os << "  ";
      else
        os << " ";
      if ((Col + 1) % m_subsize == 0 && Col < m_size - 1)
        os << "| ";
    }
    os << "\n";
    if ((Row + 1) % m_subsize == 0 && Row < m_size - 1) {
      for (auto Idx = 0u; Idx <= m_size + 1; ++Idx)
        if ((Idx + 1) % (m_subsize + 1) == 0)
          os << "+-";
        else
          os << "---";
      os << "\n";
    }
  }
}

void sudoku_solver::write_original(std::ostream &os) const {
  write(os, m_original);
}

void sudoku_solver::write_solved(std::ostream &os) const {
  assert(!m_solved.empty());
  write(os, m_solved);
}

static unsigned whichSet(const auto &Bitset, unsigned Count) {
  for (auto Bit = 0; Bit < Count; ++Bit)
    if (Bitset[Bit])
      return Bit + 1;
  throw std::logic_error("None set");
  return 0;
}

bool sudoku_solver::eliminate(board_type &Brd) const {
  int IsChange = false;
  for (auto Row = 0; Row < m_size; ++Row) {
    for (auto Col = 0; Col < m_size; ++Col) {
      auto PossibleVals = Brd.at(Row).at(Col).m_possible_values;
      if (Brd.at(Row).at(Col).m_value == 0 && PossibleVals.count() == 1) {
        IsChange = true;
        Brd.at(Row).at(Col).m_value = whichSet(PossibleVals, m_size);
        if (!reducePossibleValues(Brd, Row, Col))
          throw std::logic_error("Eleminate end bad");
      }
    }
  }
  return IsChange;
}

bool sudoku_solver::setLoneRangersRow(board_type &Brd) const {
  auto IsChange = false, ResultChange = false;

  // Repeat if changed
  do {
    IsChange = false;
    for (auto Row = 0; Row < m_size; ++Row) {
      for (auto Val = 0u; Val < m_size; ++Val) {
        int CountVals = 0u;
        int RowLR, ColLR;
        // Find in current row
        for (auto Col = 0; Col < m_size; ++Col) {
          if (Brd.at(Row).at(Col).m_value == 0 &&
              (Brd.at(Row).at(Col).m_possible_values[Val])) {
            ++CountVals;
            if (CountVals > 1)
              break;

            RowLR = Row;
            ColLR = Col;
          }
        }

        if (CountVals == 1) {
          // This value possible only in one cell in row. Set it.
          Brd.at(RowLR).at(ColLR).m_value = Val + 1;
          reducePossibleValues(Brd, RowLR, ColLR);

          IsChange = true;
          ResultChange = true;
        }
      }
    }
  } while (IsChange);
  return ResultChange;
}

bool sudoku_solver::setLoneRangersColumn(board_type &Brd) const {
  auto IsChange = false, ResultChange = false;

  // Repeat if changed
  do {
    IsChange = false;
    for (auto Col = 0; Col < m_size; ++Col) {
      for (auto Val = 0u; Val < m_size; ++Val) {
        int CountVals = 0u;
        int RowLR, ColLR;
        // Find in current col
        for (auto Row = 0; Row < m_size; ++Row) {
          if (Brd.at(Row).at(Col).m_value == 0 &&
              (Brd.at(Row).at(Col).m_possible_values[Val])) {
            ++CountVals;
            if (CountVals > 1)
              break;

            RowLR = Row;
            ColLR = Col;
          }
        }

        if (CountVals == 1) {
          // This value possible only in one cell in row. Set it.
          Brd.at(RowLR).at(ColLR).m_value = Val + 1;
          if (!reducePossibleValues(Brd, RowLR, ColLR))
            throw std::logic_error("Lone ranger row end bad");

          IsChange = true;
          ResultChange = true;
        }
      }
    }
  } while (IsChange);
  return ResultChange;

  return false;
}

bool sudoku_solver::setLoneRangersLittleSquare(board_type &Brd) const {
  auto IsChange = false, ResultChange = false;

  // Repeat if changed
  do {
    IsChange = false;
    for (auto Val = 0u; Val < m_size; ++Val) {
      for (auto SqRow = 0; SqRow < m_size; SqRow += m_subsize) {
        for (auto SqCol = 0; SqCol < m_size; SqCol += m_subsize) {
          int CountVals = 0u;
          int RowLR, ColLR;
          // Find in current little square
          for (auto Row = SqRow; Row < SqRow + m_subsize; ++Row) {
            for (auto Col = 0; Col < SqCol + m_subsize; ++Col) {
              if (Brd.at(Row).at(Col).m_value == 0 &&
                  (Brd.at(Row).at(Col).m_possible_values[Val])) {
                ++CountVals;
                if (CountVals > 1)
                  break;

                RowLR = Row;
                ColLR = Col;
              }
            }
            if (CountVals > 1)
              break;
          }

          if (CountVals == 1) {
            // This value possible only in one cell in row. Set it.
            Brd.at(RowLR).at(ColLR).m_value = Val + 1;
            if (!reducePossibleValues(Brd, RowLR, ColLR))
              throw std::logic_error("Lone ranger little square end bad");

            IsChange = true;
            ResultChange = true;
          }
        }
      }
    }
  } while (IsChange);
  return ResultChange;
}

bool sudoku_solver::setTwinsRow(board_type &Brd) const {
  auto IsChange = false;
  for (auto Row = 0; Row < m_size; ++Row) {
    for (auto Col = 0u; Col < m_size; ++Col) {
      auto PossibleVals = Brd.at(Row).at(Col).m_possible_values;
      if (PossibleVals.count() == 2) {
        // Find twin
        for (auto ColTw = Col + 1; ColTw < m_size; ++ColTw) {
          if (PossibleVals == Brd.at(Row).at(ColTw).m_possible_values) {
            // Remove the pair of possible values from all unset cells in row
            for (auto ColRem = 0; ColRem < m_size; ++ColRem) {
              if (ColRem != Col && ColRem != ColTw &&
                  Brd.at(Row).at(ColRem).m_value == 0 &&
                  (Brd.at(Row).at(ColRem).m_possible_values & PossibleVals)
                      .any()) {
                Brd.at(Row).at(ColRem).m_possible_values &= ~PossibleVals;
                IsChange = true;
              }
            }
          }
        }
      }
    }
  }
  return IsChange;
}

bool sudoku_solver::setTwinsColumn(board_type &Brd) const {
  auto IsChange = false;
  for (auto Col = 0u; Col < m_size; ++Col) {
    for (auto Row = 0; Row < m_size; ++Row) {
      auto PossibleVals = Brd.at(Row).at(Col).m_possible_values;
      if (PossibleVals.count() == 2) {
        // Find twin
        for (auto RowTw = Row + 1; RowTw < m_size; ++RowTw) {
          if (PossibleVals == Brd.at(RowTw).at(Col).m_possible_values) {
            // Remove the pair of possible values from all unset cells in col
            for (auto RowRem = 0; RowRem < m_size; ++RowRem) {
              if (RowRem != Row && RowRem != RowTw &&
                  Brd.at(RowRem).at(Col).m_value == 0 &&
                  (Brd.at(RowRem).at(Col).m_possible_values & PossibleVals)
                      .any()) {
                Brd.at(RowRem).at(Col).m_possible_values &= ~PossibleVals;
                IsChange = true;
              }
            }
          }
        }
      }
    }
  }
  return IsChange;
}

bool sudoku_solver::solveHumanistic(board_type &Brd) const {
  auto IsChange = false;
  do {
    for (auto Row = 0; Row < m_size; ++Row) {
      for (auto Col = 0; Col < m_size; ++Col)
        if (!reducePossibleValues(Brd, Row, Col))
          return false;
    }
    IsChange = eliminate(Brd);
    if (!IsChange) {
      IsChange = setLoneRangersRow(Brd);
      if (!IsChange) {
        IsChange = setLoneRangersColumn(Brd);
        if (!IsChange) {
          IsChange = setLoneRangersLittleSquare(Brd);
          if (!IsChange) {
            IsChange = setTwinsRow(Brd);
            if (!IsChange)
              IsChange = setTwinsColumn(Brd);
          }
        }
      }
    }
  } while (IsChange);
  if (!isCorrectInput(Brd))
    return false;

  return true;
}

void sudoku_solver::pushIdxPermutations(const std::pair<int, int> &Idx,
                                        board_type Brd,
                                        std::vector<board_type> *Stack) const {
  auto [Row, Col] = Idx;
  auto PossibleVals = Brd.at(Row).at(Col).m_possible_values;
  Brd.m_row = Row;
  Brd.m_col = Col;
  auto Val = 1;
  do {
    if (PossibleVals[0]) {
      Brd.at(Row).at(Col).m_value = Val;
      reducePossibleValues(Brd, Row, Col);
      if (isCorrectInput(Brd))
        Stack->push_back(Brd);
    }
    PossibleVals >>= 1;
    ++Val;
  } while (PossibleVals.count());
}

bool sudoku_solver::solveBruteForce(board_type &Brd) {
#pragma omp parallel shared(m_solution_found, m_solved)
  {
    std::vector<board_type> LocalStack;
    board_type CurrentBrd;

    auto ThreadsNum = omp_get_num_threads();
    for (auto NumGrid = omp_get_thread_num();
         NumGrid < permutations_stack_g.size() && !m_solution_found;
         NumGrid += ThreadsNum) {
      CurrentBrd = permutations_stack_g[NumGrid];
      LocalStack.push_back(CurrentBrd);

      do {
        while (!solveHumanistic(CurrentBrd) && !LocalStack.empty()) {
          CurrentBrd = LocalStack.back();
          LocalStack.pop_back();
        }

        // Search next cell
        auto [Row, Col] = getLeastUnsureCell(CurrentBrd);
        if (Row == m_size) {
#pragma omp critical
          {
            m_solution_found = true;
            m_solved = CurrentBrd;
          }
          break;
        }

        pushIdxPermutations(std::pair(Row, Col), CurrentBrd, &LocalStack);

        if (!LocalStack.empty()) {
          CurrentBrd = LocalStack.back();
          LocalStack.pop_back();
        }
      } while (!LocalStack.empty() && !m_solution_found);
    }
  }
  return true;
}

bool sudoku_solver::fillPermutationStack(board_type &Brd) {
  board_type CurrentBrd;

  std::vector<board_type> Stack1Data, Stack2Data;
  std::vector<board_type> *Stack1, *Stack2;

  Brd.m_row = 0;
  Brd.m_col = 0;
  Stack1Data.push_back(Brd);

  auto Step = 0;
  do {
    if (Step % 2) {
      Stack1 = &Stack2Data;
      Stack2 = &Stack1Data;
    } else {
      Stack1 = &Stack1Data;
      Stack2 = &Stack2Data;
    }
    while (!Stack1->empty()) {
      CurrentBrd = Stack1->back();
      Stack1->pop_back();

      // Search next cell
      auto [Row, Col] = getLeastUnsureCell(CurrentBrd);
      if (Row == m_size) {
        m_solution_found = true;
        m_solved = CurrentBrd;
        return true;
      }
      pushIdxPermutations(std::pair(Row, Col), CurrentBrd, Stack2);
    }

    // No solutions
    if (Stack2->empty())
      return false;
    ++Step;
  } while (Stack2->size() < stack_size_g);

  permutations_stack_g.resize(Stack2->size());
  permutations_stack_g = *Stack2;

  return true;
}

std::pair<int, int>
sudoku_solver::getLeastUnsureCell(const board_type &Brd) const {
  std::pair<int, int> Idx = {0, 0};
  int Min = m_size + 1;
  bool IsExist = false;
  for (auto Row = 0; Row < m_size; ++Row) {
    for (auto Col = 0; Col < m_size; ++Col) {
      if (Brd.at(Row).at(Col).m_value == 0 &&
          Brd.at(Row).at(Col).m_possible_values.count() < Min) {
        IsExist = true;
        Idx = {Row, Col};
        Min = Brd.at(Row).at(Col).m_possible_values.count();
      }
    }
  }
  if (!IsExist)
    return {m_size, m_size};
  assert(Min < m_size + 1);
  return Idx;
}

bool sudoku_solver::solve() {
  board_type Startboard_type = m_original;

  // Setting all possible values
  for (auto Row = 0; Row < m_size; ++Row) {
    for (auto Col = 0; Col < m_size; ++Col)
      setPossibleValues(Startboard_type, Row, Col);
  }

  Startboard_type.m_row = 0;
  Startboard_type.m_col = 0;

  // First part -- humanistic algorithm
  if (!solveHumanistic(Startboard_type))
    return false;

  // If solved -> return true
  if (is_solved(Startboard_type)) {
    m_solved = Startboard_type;
    return true;
  }

  if (!fillPermutationStack(Startboard_type))
    return false;
  if (m_solution_found)
    return true;
  // If the humanistic algorithm returns a board with unfilled
  // cells left, then we pass it to the brute force algorithm
  if (!solveBruteForce(Startboard_type))
    return false;

  // If solved -> return true
  if (m_solution_found && is_solved(m_solved))
    return true;
  return false;
}

void sudoku_solver::setPossibleValues(board_type &Brd, value_type Row,
                                      value_type Col) const {
  auto Value = Brd.at(Row).at(Col).m_value;
  if (Value == 0) {
    for (auto Val = 1u; Val <= m_size; ++Val)
      Brd.at(Row).at(Col).m_possible_values.set(Val - 1);
    return;
  }
  Brd.at(Row).at(Col).m_possible_values.set(Value - 1);
}

bool sudoku_solver::reducePossibleValues(board_type &Brd, value_type Row,
                                         value_type Col) const {
  assert(Brd.size() == m_size);
  if (Brd.at(Row).at(Col).m_value > 0) {
    Brd.at(Row).at(Col).m_possible_values.reset();
    Brd.at(Row).at(Col).m_possible_values.set(Brd.at(Row).at(Col).m_value - 1);
    assert(Brd.at(Row).at(Col).m_possible_values.count() == 1);
    return true;
  }
  auto SqRowSt = (Row / m_subsize) * m_subsize,
       SqColSt = (Col / m_subsize) * m_subsize;
  for (auto Idx = 0; Idx < m_size; Idx++) {
    auto SqRow = Idx / m_subsize;
    auto SqCol = Idx % m_subsize;
    auto Value = Brd.at(Row).at(Idx).m_value;
    // Check row
    if (Value > 0)
      Brd.at(Row).at(Col).m_possible_values.reset(Value - 1);

    Value = Brd.at(Idx).at(Col).m_value;
    // Check column
    if (Value > 0)
      Brd.at(Row).at(Col).m_possible_values.reset(Value - 1);

    Value = Brd.at(SqRowSt + SqRow).at(SqColSt + SqCol).m_value;
    // Check little square
    if (Value > 0)
      Brd.at(Row).at(Col).m_possible_values.reset(Value - 1);
  }
  if (Brd.at(Row).at(Col).m_possible_values.none())
    return /* No possible values */ false;
  return true;
}

} // namespace sudoku
