#include <mpi.h>
#include <string>
#include <chrono>
#include <vector>
#include <gmpxx.h>
#include <iostream>
#include <stdexcept>

//mpic++ -std=c++17 task.cpp -lgmp -lgmpxx -O3 -o a1.out
//mpirun --map-by :oversubscribe -n 2 a.out 100000 t

enum class what_show: bool
{
  TIME,
  EXP,
};

using buffer = std::vector<char>;

static auto factorial(size_t n) -> mpz_class {
  mpz_class fact = 1U;
  for (size_t i = 1U; i <= n; ++i) {
    fact *= i;
  }
  return fact;
}

static auto calculate(size_t from, size_t to, size_t max, int rank) -> mpz_class {
  mpz_class sum = 0U, step = 1U;
  for (size_t i = max; i > to; --i) {
    step *= i;
  }
  // std::cout << "max " << max << "step " << step << std::endl;
  for (size_t i = to; i != from; --i) {
    step *= i;
    sum += step;
  }
  // std::cout << "Node " << rank << ": Got interval: [" << from << ", " << to
  // << "). The sum between is " << sum << std::endl;
  return sum;
}

static auto recv_mpz(int src_rank) -> mpz_class {
  MPI_Status status;
  mpz_class number = 0;
  int n_bytes = 0;
  buffer buf;

  MPI_Probe(src_rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  MPI_Get_count(&status, MPI_BYTE, &n_bytes);
  buf.resize(n_bytes*2);
  MPI_Recv(buf.data(), n_bytes, MPI_BYTE, src_rank, MPI_ANY_TAG, MPI_COMM_WORLD,
           &status);
  mpz_import(number.get_mpz_t(), n_bytes, 1, sizeof(char), 0, 0, buf.data());
  return number;
}

static auto send_mpz(int dst_rank, mpz_ptr number) -> void {
  size_t count;
  void *buf = mpz_export(nullptr, &count, 1, sizeof(char), 0, 0, number);
  MPI_Ssend(buf, count, MPI_BYTE, dst_rank, 0, MPI_COMM_WORLD);
}

static auto start_job(int rank, int n_nodes, int precise, what_show what) -> void {
  // std::cout << "Node " << rank << ": Started..." << std::endl;
  mpf_set_default_prec(64 + ceil(3.33 * precise));

  size_t n_iters = precise;
  size_t from = n_iters / n_nodes * rank;
  size_t to = n_iters / n_nodes * (rank + 1);
  size_t max = n_iters;
  if (rank == n_nodes - 1) {
    to += n_iters % n_nodes;
  }
  auto part_sum = calculate(from, to, max, rank);
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    for (int i = 1; i != n_nodes; ++i) {
      auto tmp = recv_mpz(i);
      part_sum += tmp;
    }
    part_sum++;
    auto denom = factorial(max);
    mpf_class res = part_sum;
    res /= denom;
    //std::cout << part_sum.get_mpz_t() << std::endl;
    //std::cout << denom.get_mpz_t() << std::endl;
    // std::cout << res.get_mpf_t() << std::endl;
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<microseconds>(end - start).count();
    if (what == what_show::TIME) {
      std::cout << elapsed << std::endl;
    } else {
      gmp_printf("%.*Ff\n", precise, res.get_mpf_t());
    }
  } else {
    send_mpz(0, part_sum.get_mpz_t());
  }
}

auto main(int argc, char *argv[]) -> int {
  try {
  if (argc != 3) {
    return -1;
  }
  std::string str = argv[2];
  what_show what;
  if (str == "t") {
    what = what_show::TIME;
  } else if (str == "e") {
    what = what_show::EXP;
  } else {
    throw std::logic_error("Unknown 2nd argument");
  }
  MPI_Init(&argc, &argv);
  auto size = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  auto n_iters = atoi(argv[1]);
  start_job(rank, size, n_iters, what);
  MPI_Finalize();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
