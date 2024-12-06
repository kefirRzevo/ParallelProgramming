#include <cmath>
#include <vector>
#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <omp.h>
#include <numeric>
#include <boost/program_options.hpp>

constexpr const size_t m = 14;
constexpr const size_t N = (1 << m) - 1;
constexpr const size_t numIterations = 5000;
constexpr const size_t bVarianceNum = 11;
constexpr const double h = 1.0 / N;

namespace fs = std::filesystem;
namespace po = boost::program_options;

auto repo_path = fs::path{__FILE__}.parent_path().parent_path();

auto defaultDataPath = repo_path / "build" / "OMP_Equation" / "data.txt";

/*
 * Solve system of linear algebraic equations:
 *
 * Ax = f
 *
 * where A is threediagonal matrix with diagonals a, b and c
 *
 */

using data_t = std::vector<double>;

void solveThreediagonalSLAE(data_t& a, data_t& b, data_t& c, data_t& f, data_t& x) {

#pragma omp parallel
  {
    // Forward pass
    auto stride = uint64_t{1};
    for (auto nn = uint64_t{N}, low = uint64_t{2}; nn > 1; nn /= 2, low *= 2, stride *= 2) {
#pragma omp for
      for (auto i = low - 1; i < N; i += stride * 2) {
        double alpha = -a[i] / b[i - stride];
        double gamma = -c[i] / b[i + stride];
        a.at(i) = alpha * a[i - stride];
        b.at(i) = alpha * c[i - stride] + b[i] + gamma * a[i + stride];
        c.at(i) = gamma * c[i + stride];
        f.at(i) = alpha * f[i - stride] + f[i] + gamma * f[i + stride];
      }
    }

#pragma omp barrier

    // Reverse pass
    x[N / 2] = f[N / 2] / b[N / 2];
    for (stride /= 2; stride >= 1; stride /= 2) {
#pragma omp for
      for (uint64_t i = stride - 1; i < N; i += stride * 2) {
        x[i] = (f[i] - (i - stride > 0 ? a[i] * x[i - stride] : 0.0) -
                (i + stride < N ? c[i] * x[i + stride] : 0.0)) /
               b[i];
      }
    }
  }
}

double calculateResidue(const data_t& yPrev, const data_t& yCurr) {
  auto res = double{0};
  for (auto i = size_t{0}; i < N; ++i)
    res += std::fabs(yPrev.at(i) - yCurr.at(i));
  return res;
}

int main(int argc, char *argv[]) {
  auto num_threads = int{};
  auto epsilon = double{};
  auto outFilename = std::string{};

  auto desc = po::options_description{"allowed options"};
  desc.add_options()("help", "produce this help message")
    ("output,o", po::value(&outFilename)->default_value(defaultDataPath.c_str()), "output file path")
    ("epsilon,e", po::value(&epsilon)->default_value(1e-32), "epsilon")
    ("threads,t", po::value(&num_threads)->default_value(int32_t{1}), "threads count");
  auto vm = po::variables_map{};
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }
  po::notify(vm);

  omp_set_num_threads(num_threads);
  omp_set_dynamic(0);

  data_t x; x.resize(N);
  data_t yPrev;
  yPrev.resize(N);
  data_t yCurr;
  yCurr.resize(N);
  data_t a;
  a.resize(N);
  data_t b;
  b.resize(N);
  data_t c;
  c.resize(N);
  data_t f;
  f.resize(N);

  std::vector<data_t> solution_of_b{bVarianceNum};
  for (auto&& solution:solution_of_b)
    solution.resize(N);

  for (int sol = 0; sol < bVarianceNum; ++sol) {
    double rightBound = sol * 1.0 / (bVarianceNum - 1);

    // Initial approximation is: y = 1 + (b - 2) * x + x^2
    for (size_t i = 0; i < N; ++i) {
      const double currX = i * h;
      x.at(i) = currX;
      yPrev.at(i) = 1 + (rightBound - 2) * currX + currX * currX;
    }

    auto start = omp_get_wtime();
    for (size_t iters = 0; iters < numIterations; ++iters) {
      constexpr const double h2_over_12 = h * h / 12.0;

      a.at(0) = 0;
      b.at(0) = 1;
      c.at(0) = 0;
      f.at(0) = 1;

#pragma omp parallel for schedule(static, 3)
      for (size_t k = 1; k < N - 1; ++k) {
        const auto exp_y_k_minus_1 = std::exp(yPrev[k - 1]);
        const auto exp_y_k = std::exp(yPrev[k]);
        const auto exp_y_k_plus_1 = std::exp(yPrev[k + 1]);

        a.at(k) = 1.0 - h2_over_12 * exp_y_k_plus_1;
        b.at(k) = -2.0 - 10 * h2_over_12 * exp_y_k;
        c.at(k) = 1.0 - h2_over_12 * exp_y_k_minus_1;
        f.at(k) = h2_over_12 * (exp_y_k_plus_1 * (1.0 - yPrev[k + 1]) +
                             10.0 * exp_y_k * (1.0 - yPrev[k]) +
                             exp_y_k_minus_1 * (1.0 - yPrev[k - 1]));
      }
      a.at(N - 1) = 0;
      b.at(N - 1) = 1;
      c.at(N - 1) = 0;
      f.at(N - 1) = rightBound;
      solveThreediagonalSLAE(a, b, c, f, yCurr);
      if (calculateResidue(yPrev, yCurr) < epsilon)
        break;
      yPrev = yCurr;
    }
    auto end = omp_get_wtime();

    solution_of_b[sol] = yCurr;

    std::cout << "b = " << rightBound << std::endl;
    std::cout << "epsilon = " << std::scientific << epsilon << std::defaultfloat
              << std::endl;
    std::cout << "Number of executors: " << num_threads << std::endl;
    std::cout << "Elapsed time: " << end - start << " seconds\n" << std::endl;
  }

  auto outFile = std::ofstream{outFilename};
  for (int i = 0; i < N; ++i) {
    outFile << x[i];
    for (int sol = 0; sol < bVarianceNum; ++sol) {
      outFile << " " << solution_of_b.at(sol).at(i);
    }
    outFile << std::endl;
  }
  return 0;
}
