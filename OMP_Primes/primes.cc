#include <boost/dynamic_bitset.hpp>
#include <boost/program_options.hpp>

#include <algorithm>
#include <atomic>
#include <bit>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <omp.h>

namespace po = boost::program_options;

constexpr bool simple_check(uint64_t number) {
  const auto simples = {2, 3, 5, 7, 11, 13, 17, 19, 23};
  return std::any_of(simples.begin(), simples.end(), [=](uint64_t divisor) {
    if (number >= divisor * divisor && number % divisor == 0)
      return true;
    return false;
  });
}

uint64_t get_sqrt_upper_bound(uint64_t n) {
  if (n < 2)
    throw std::invalid_argument{"number can't be smaller than 2"};
  auto msb_index = sizeof(uint64_t) * CHAR_BIT - std::countl_zero(n) - 1;
  auto upper_bound_pow_2 = (msb_index + 1);
  auto sqrt_pow_upper_bound = (upper_bound_pow_2 / 2) + (upper_bound_pow_2 % 2);
  return (uint64_t{1} << sqrt_pow_upper_bound);
}

template <typename Primes>
void process(Primes &primes, uint64_t from, uint64_t to) {
  for (auto i = uint64_t{2}; i * i <= to; ++i) {
    if (simple_check(i))
      continue;

    auto start_j = std::max(((from + i - 1) / i) * i, i * i);
    for (auto j = start_j; j <= to; j += i)
      primes[j] = false;
  }
}

boost::dynamic_bitset<> find_sequential(uint64_t n) {
  auto primes = boost::dynamic_bitset(n);
  primes.set();

  for (auto i = uint64_t{2}; i < n; i++)
    process(primes, i, std::min(i + 1, n - 1));

  return primes;
}

std::vector<std::atomic<bool>> find_parallel(uint64_t n) {
  auto primes = std::vector<std::atomic<bool>>(n);

#pragma omp parallel for
  for (auto i = uint64_t{0}; i < primes.size(); ++i)
    primes[i].store(true);

#pragma omp parallel for schedule(dynamic)
  for (auto i = uint64_t{2}; i < n; i++)
    process(primes, i, std::min(i + 1, n));

  return primes;
}

int main(int argc, const char **argv) {
  auto desc = po::options_description{"allowed options"};
  auto n = uint64_t{0};

  auto mode = std::string{};
  auto threads = int32_t{};
  auto need_print = false;

  desc.add_options()("help", "produce this help message")(
      "num,n", po::value(&n)->default_value(uint64_t{1} << 20),
      "max number to check")("mode", po::value(&mode)->default_value("par"))(
      "print", po::bool_switch(&need_print)->default_value(false))(
      "threads,t", po::value(&threads)->default_value(int32_t{1}),
      "threads count for parallel mode");

  auto vm = po::variables_map{};
  po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return EXIT_FAILURE;
  }

  po::notify(vm);

  auto print = [&](auto &&cont) {
    if (!need_print)
      return;
    for (auto i = uint64_t{2}; i < cont.size(); ++i) {
      if (cont[i])
        std::cout << i << "\n";
    }
  };

  if (mode == "par") {
    omp_set_num_threads(threads);
    auto primes = find_parallel(n);
    print(primes);
  } else if (mode == "seq") {
    auto primes = find_sequential(n);
    print(primes);
  } else {
    auto ss = std::stringstream{};
    ss << "unknown mode " << std::quoted(mode);
    throw std::invalid_argument{std::move(ss).str()};
  }
}
