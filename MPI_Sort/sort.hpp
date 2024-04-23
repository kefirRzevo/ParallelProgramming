
#pragma once

#include <mpi.h>
#include <random>
#include <string>
#include <limits>
#include <fstream>
#include <numeric>
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace detail {

/********** Merge Function **********/
template<typename T>
void merge(std::vector<T> &a, std::vector<T> &b, int l, int m, int r) {
  int h, i, j, k;
  h = l;
  i = l;
  j = m + 1;
  while ((h <= m) && (j <= r)) {
    if (a[h] <= a[j]) {
      b[i] = a[h];
      h++;
    } else {
      b[i] = a[j];
      j++;
    }
    i++;
  }
  if (m < h) {
    for (k = j; k <= r; k++) {
      b[i] = a[k];
      i++;
    }
  } else {
    for (k = h; k <= m; k++) {
      b[i] = a[k];
      i++;
    }
  }
  std::copy(b.begin() + l, b.begin() + r + 1, a.begin() + l);
}

/********** Recursive Merge Function **********/
template<typename T>
void merge_sort(std::vector<T> &a, std::vector<T> &b, int l, int r) {
  int m;
  if (l < r) {
    m = (l + r) / 2;
    merge_sort(a, b, l, m);
    merge_sort(a, b, (m + 1), r);
    merge(a, b, l, m, r);
  }
}

}

template<typename T>
inline size_t read(std::vector<T> &data, std::istream &is) {
  is.seekg(0, std::ios::beg);
  T temp;
  while (is >> temp) {
    data.emplace_back(temp);
  }
  return data.size();
}

template<typename T>
inline void write(const std::vector<T>& data, std::ostream& os) {
    os << data.size() << std::endl;
    for (auto&& elem: data) {
        os << elem << " ";
    }
    os << std::endl;
}

template<typename T>
inline std::ostream &operator<<(std::ostream &os, const std::vector<T> &buf) {
  for (auto &&elem : buf) {
    std::cout << elem << " ";
  }
  return os;
}

template<typename RandomIt>
inline void merge_sort_seq(RandomIt start, RandomIt finish) {
  if (start == finish || finish == std::next(start))
    return;
  auto middle = start + (finish - start) / 2;
  merge_sort_seq(start, middle);
  merge_sort_seq(middle, finish);
  std::inplace_merge(start, middle, finish);
}
