/*
            du/dt + a * du/dx = f(x, t),     0 <= t <= T, 0 <= x <= X
    System  u(x, 0) = phi(x),                0 <= x <= X
            u(0, t) = psi(t),                0 <= t <= T

      t = k * tau, 0 <= k <= K
      x = m * h,   0 <= m <= M

      K * tau = T
      M * h   = X

      (u[k+1][m-1] - u[k][m-1] + u[k+1][m] - u[k][m]) / (2*tau) + (u[k+1][m] -
      u[k+1][m-1] + u[k][m] - u[k][m-1]) / (2*h) = f[k+1/2][m+1/2]

      k = k + 1

      (u[k][m-1] - u[k-1][m-1] + u[k][m] - u[k-1][m]) / (2*tau) + (u[k][m] -
      u[k][m-1] + u[k-1][m] - u[k-1][m-1]) / (2*h) = f[k-1/2][m+1/2]

    It can be visualized so (assuming t axis goes up and x right):

      (k, m-1) _       _ (k, m)
              |_|<--->|_|
               ^       ^
               |       |
               _       _
              |_|<--->|_|
    (k-1, m-1)           (k-1, m)

    Thus, we start from left bottom corner of the mesh and every step evaluate
    (k, m) node from (k-1, m-1), (k-1, m) and (k, m-1) nodes.

    Sequential algorithm is straight forward: we will go row by row until work
    is done. Every step requires knowledge of (k-1, m-1), (k-1, m) and (k, m-1)
    nodes. But since we go row by row, we already know these values for each
   step.

      t |_____________________________________
        |     |     |     |     |     |     |_
        |  >  |  >  |  >  |  >  |  >  |  >  |_
        |_____|_____|_____|_____|_____|_____|_
        |     |     |     |     |     |     |_
    K   |  >  |  >  |  >  |  >  |  >  |  >  |_
        |_____|_____|_____|_____|_____|_____|_
        |     |     |     |     |     |     |_
        |  >  |  >  |  >  |  >  |  >  |  >  |_
        |_____|_____|_____|_____|_____|_____|__
       0                                          x
                          M

      t |
        |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_
        |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_
        |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_
        |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_
     K  |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_
        |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_
        |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_
        |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_
        |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|_| |_|_|_|__
       0  proc0     proc1     proc2     proc3     ...    x
                                M

    Parallel algorithm will be implemented using MPI (Message Passing
    Interface).

    Note that in case of only 1 process the parallel algorithm degenerates to
    sequential one without even overhead costs. So, it becomes purely sequential
    algorithm.
*/

#include <cmath>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <numbers>
#include <stdexcept>
#include <vector>

// mpic++ -std=c++17 task.cpp -o build/MPI_Lab/MPI_Lab_Task
// mpirun --map-by :oversubscribe -n 2 build/MPI_Lab/MPI_Lab_Task

// Define some problem specific constants
constexpr int K = 400;
constexpr int M = 400;

constexpr double a = 2.0;

constexpr double T = 1;
constexpr double X = 1;

constexpr double tau = T / K;
constexpr double h = X / M;

constexpr double c_1 = 2 * tau * h / (a * tau + h);
constexpr double c_2 = (a * tau - h) / (a * tau + h);

constexpr double pi = 3.141592653589793238462643383279502884L;
constexpr int str_size = 25;

// Boundary and initial fuctions
constexpr auto phi = [](double x) -> double { return std::cos(pi * x); };
constexpr auto psi = [](double t) -> double { return std::exp(-t); };
constexpr auto f = [](double x, double t) -> double { return x + t; };

int main(int argc, char *argv[]) {
  try {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto start = MPI_Wtime();

    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, "res/output.txt",
                  MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &file);
    MPI_File_set_size(file, 0);

    if (rank == 0) {
      auto init = "t       x       u       \n";
      MPI_File_write_at(file, 0, init, str_size, MPI_CHAR, MPI_STATUS_IGNORE);
    }

    int bsend_size;

    MPI_Pack_size(2 * K, MPI_DOUBLE, MPI_COMM_WORLD, &bsend_size);
    std::vector<double> bsend_buf;
    bsend_buf.resize(bsend_size);

    MPI_Buffer_attach(bsend_buf.data(), bsend_size);

    auto m_diff = M / size;
    auto m_first = m_diff * rank;
    auto m_last = m_diff * (rank + 1);
    if (M % size != 0) {
      if (rank < M % size) {
        m_first += rank;
        m_last += rank + 1;
      } else {
        m_first += (M % size);
        m_last += (M % size);
      }
    }
    auto m_total = m_last - m_first;

    auto dump = [&](int k, int m, double func) -> void {
      constexpr auto to_string = [](double t, double x,
                                    double u) -> std::string {
        // no std::format on macos :(
        // P. S. I know this is very bad
        std::string temp;
        temp.resize(str_size);
        snprintf(const_cast<char *>(temp.c_str()), 27, "%-8.5f%-8.5f%-8.5f\n",
                 t, x, u);
        return temp;
      };
      constexpr auto get_offset = [](double k, double m) -> int {
        return (k * M + m + 1) * str_size;
      };
      double t = k * tau;
      double x = m * h;
      double u = func;
      auto report = to_string(t, x, u);
      MPI_File_write_at(file, get_offset(k, m), report.c_str(), str_size,
                        MPI_CHAR, MPI_STATUS_IGNORE);
    };

    std::vector<double> prev_row, curr_row;
    prev_row.resize(m_total);
    curr_row.resize(m_total);

    MPI_Barrier(MPI_COMM_WORLD);

    for (int m = 0; m < m_total; m++) {
      prev_row[m] = phi((m_first + m) * h);
      dump(0, m_first + m, prev_row[m]);
    }

    if (rank != 0) {
      double prev_col_curr_row = 0;
      double prev_col_prev_row = phi((m_first - 1) * h);

      for (int k = 1; k < K; k++) {
        MPI_Recv(&prev_col_curr_row, 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        curr_row[0] = c_1 * f((m_first + 0.5) * h, (k - 0.5) * tau) +
                      prev_col_prev_row +
                      (prev_col_curr_row - prev_row[0]) * c_2;
        dump(k, m_first, curr_row[0]);
        for (int m = 1; m < m_total; m++) {
          curr_row[m] = c_1 * f((m_first + m + 0.5) * h, (k - 0.5) * tau) +
                        prev_row[m - 1] + (curr_row[m - 1] - prev_row[m]) * c_2;
          dump(k, m_first + m, curr_row[m]);
        }
        std::swap(curr_row, prev_row);
        prev_col_prev_row = prev_col_curr_row;
        if (rank < size - 1) {
          MPI_Bsend(&prev_row[m_first - 1], 1, MPI_DOUBLE, rank + 1, 0,
                    MPI_COMM_WORLD);
        }
      }
    } else {
      for (int k = 1; k < K; k++) {
        curr_row[0] = psi(k * tau);
        dump(k, 0, curr_row[0]);

        for (int m = 1; m < m_total; m++) {
          curr_row[m] = c_1 * f((m + 0.5) * h, (k - 0.5) * tau) +
                        prev_row[m - 1] + (curr_row[m - 1] - prev_row[m]) * c_2;
          dump(k, m, curr_row[m]);
        }

        std::swap(curr_row, prev_row);
        if (size > 1) {
          MPI_Bsend(&prev_row[m_total - 1], 1, MPI_DOUBLE, rank + 1, 0,
                    MPI_COMM_WORLD);
        }
      }
    }
    MPI_Buffer_detach(&bsend_buf, &bsend_size);
    MPI_File_close(&file);
    if (rank == 0) {
      auto end = MPI_Wtime();
      std::cout << end - start << std::endl;
    }
    MPI_Finalize();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
