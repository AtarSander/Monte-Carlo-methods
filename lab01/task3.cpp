#include <exception>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils.hpp"


template <typename Engine>
BenchmarkResult benchmark_engine(const std::string& name,
                                 std::size_t count,
                                 double a,
                                 double b,
                                 typename Engine::result_type seed) {
    Engine eng(seed);
    std::uniform_real_distribution<double> dist(a, b);

    volatile double sink = 0.0;

    auto start = now();
    for (std::size_t i = 0; i < count; ++i) {
        sink += dist(eng);
    }
    auto end = now();

    (void)sink;

    return {name, us_between(start, end)};
}


std::vector<BenchmarkResult> benchmark_all_engines(std::size_t count,
                                                   double low,
                                                   double high,
                                                   int seed) {
    std::vector<BenchmarkResult> results;

    results.push_back(
        benchmark_engine<std::minstd_rand>(
            "minstd_rand", count, low, high,
            static_cast<std::minstd_rand::result_type>(seed)));

    results.push_back(
        benchmark_engine<std::mt19937>(
            "mt19937", count, low, high,
            static_cast<std::mt19937::result_type>(seed)));

    results.push_back(
        benchmark_engine<std::mt19937_64>(
            "mt19937_64", count, low, high,
            static_cast<std::mt19937_64::result_type>(seed)));

    results.push_back(
        benchmark_engine<std::ranlux48_base>(
            "ranlux48_base", count, low, high,
            static_cast<std::ranlux48_base::result_type>(seed)));

    results.push_back(
        benchmark_engine<std::ranlux48>(
            "ranlux48", count, low, high,
            static_cast<std::ranlux48::result_type>(seed)));

    results.push_back(
        benchmark_engine<std::knuth_b>(
            "knuth_b", count, low, high,
            static_cast<std::knuth_b::result_type>(seed)));

    return results;
}


int main(int argc, char* argv[]) {
    try {
        if (argc != 6) {
            std::cerr << "Usage: " << argv[0] << " <min_milliseconds> <low> <high> <seed> <filepath>\n";
            return 1;
        }

        const long long min_microseconds = static_cast<long long>(std::stod(argv[1]) * 1000.0);
        const double low = std::stod(argv[2]);
        const double high = std::stod(argv[3]);
        const int seed = std::stoi(argv[4]);
        const std::string filepath = argv[5];

        if (min_microseconds <= 0) {
            throw std::invalid_argument("min_milliseconds must be positive");
        }

        std::size_t count = 1000;
        std::vector<BenchmarkResult> results = benchmark_all_engines(count, low, high, seed);

        while (shortest_time(results) < min_microseconds) {
            count *= 2;
            results = benchmark_all_engines(count, low, high, seed);
        }

        std::cout << std::left << std::setw(16) << "generator"
                  << std::right << std::setw(16) << "count"
                  << std::setw(16) << "time [us]\n";
        std::cout << "------------------------------------------------\n";

        for (const auto& r : results) {
            std::cout << std::left << std::setw(16) << r.name
                      << std::right << std::setw(16) << count
                      << std::setw(16) << r.microseconds << "\n";
        }

        save_results_csv(results, filepath);
        std::cout << "\nSaved results to " << filepath << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
