#include <boost/dynamic_bitset.hpp>
#include <boost/program_options.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <deque>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <omp.h>

namespace options = boost::program_options;

constexpr bool is_composite(uint64_t number) {
  const auto simples = {2, 3, 5, 7, 11, 13, 17, 19, 23};
  return std::any_of(simples.begin(), simples.end(), [=](uint64_t divisor) {
    if (number >= divisor * divisor && number % divisor == 0)
      return true;
    return false;
  });
}

uint64_t calculate_sqrt_upper_limit(uint64_t value) {
  if (value < 2)
    throw std::invalid_argument{"value must be at least 2"};

  auto highest_bit = sizeof(uint64_t) * CHAR_BIT - std::countl_zero(value) - 1;
  auto upper_bound_exponent = (highest_bit + 1);
  auto sqrt_exponent = (upper_bound_exponent / 2) + (upper_bound_exponent % 2);

  return (uint64_t{1} << sqrt_exponent);
}

void process(auto &prime_flags, uint64_t i, uint64_t start, uint64_t end) {
  if (is_composite(i))
    return;

  auto j_start = std::max(((start + i - 1) / i) * i, i * i);
  for (auto j = j_start; j <= end; j += i)
    prime_flags[j] = false;
}

boost::dynamic_bitset<> sequential_prime_finding(uint64_t limit) {
  auto prime_flags = boost::dynamic_bitset(limit);
  prime_flags.set();
  for (auto i = uint64_t{2}; i * i <= limit; ++i)
    process(prime_flags, i, uint64_t{2}, limit);
  return prime_flags;
}

std::vector<std::atomic<bool>> parallel_prime_finding(uint64_t limit) {
  auto prime_flags = std::vector<std::atomic<bool>>(limit);

#pragma omp parallel for
  for (auto i = uint64_t{0}; i < prime_flags.size(); ++i)
    prime_flags[i].store(true);

#pragma omp parallel for schedule(dynamic)
  for (auto i = uint64_t{2}; i <= calculate_sqrt_upper_limit(limit); ++i)
    process(prime_flags, i, uint64_t{2}, limit);

  return prime_flags;
}

int main(int argc, const char **argv) {
  auto description = options::options_description{"available options"};
  auto limit = uint64_t{0};

  auto execution_mode = std::string{};
  auto should_print = false;
  auto threads_count = u_int16_t{1};

  description.add_options()("help", "display this help message")(
      "num,n", options::value(&limit)->default_value(uint64_t{1} << 20),
      "maximum number to evaluate")(
      "mode", options::value(&execution_mode)->default_value("parallel"))(
      "print", options::bool_switch(&should_print)->default_value(false))(
      "threads count,t", options::value(&threads_count)->default_value(1));

  auto variable_map = options::variables_map{};
  options::store(
      options::command_line_parser(argc, argv).options(description).run(),
      variable_map);

  if (variable_map.count("help")) {
    std::cout << description << "\n";
    return 0;
  }

  options::notify(variable_map);

  auto print_primes = [&](auto &&container) {
    if (!should_print)
      return;
    for (auto i = uint64_t{2}; i < container.size(); ++i) {
      if (container[i])
        std::cout << i << "\n";
    }
  };

  omp_set_num_threads(threads_count);
  if (execution_mode == "par") {
    omp_set_num_threads(threads_count);
    auto prime_flags = parallel_prime_finding(limit);
    print_primes(prime_flags);
  } else if (execution_mode == "seq") {
    auto prime_flags = sequential_prime_finding(limit);
    print_primes(prime_flags);
  } else {
    std::stringstream error_stream{};
    error_stream << "unrecognized mode " << std::quoted(execution_mode);
    throw std::invalid_argument{std::move(error_stream).str()};
  }
}
