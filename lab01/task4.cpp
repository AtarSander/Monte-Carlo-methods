#include <gsl/gsl_rng.h>

#include <cstddef>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils.hpp"


BenchmarkResult benchmark_gsl_rng(const gsl_rng_type* type,
                                  std::size_t count,
                                  unsigned long seed) {
    gsl_rng* generator = gsl_rng_alloc(type);
    if (generator == nullptr) {
        throw std::runtime_error("Cannot allocate GSL generator: " + std::string(type->name));
    }

    gsl_rng_set(generator, seed);

    volatile double sink = 0.0;

    auto start = now();
    for (std::size_t i = 0; i < count; ++i) {
        sink += gsl_rng_uniform(generator);
    }
    auto end = now();

    gsl_rng_free(generator);
    (void)sink;

    return {type->name, us_between(start, end)};
}


std::vector<BenchmarkResult> benchmark_all_gsl_rngs(std::size_t count, unsigned long seed) {
    const gsl_rng_type** types = gsl_rng_types_setup();
    std::vector<BenchmarkResult> results;

    for (const gsl_rng_type** type = types; *type != nullptr; ++type) {
        results.push_back(benchmark_gsl_rng(*type, count, seed));
    }

    return results;
}


int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <min_milliseconds> <seed> <filepath>\n";
            return 1;
        }

        const long long min_microseconds = static_cast<long long>(std::stod(argv[1]) * 1000.0);
        const unsigned long seed = std::stoul(argv[2]);
        const std::string filepath = argv[3];

        if (min_microseconds <= 0) {
            throw std::invalid_argument("min_milliseconds must be positive");
        }

        std::size_t count = 1000;
        std::vector<BenchmarkResult> results = benchmark_all_gsl_rngs(count, seed);

        while (shortest_time(results) < min_microseconds) {
            count *= 2;
            results = benchmark_all_gsl_rngs(count, seed);
        }

        const long long default_time = benchmark_gsl_rng(gsl_rng_default, count, seed).microseconds;

        std::cout << std::left << std::setw(24) << "generator"
                  << std::right << std::setw(16) << "count"
                  << std::right << std::setw(16) << "time [us]"
                  << std::setw(24) << "relative to default" << '\n';
        std::cout << "--------------------------------------------------------------------------------\n";

        for (const auto& result : results) {
            const double relative_time = static_cast<double>(result.microseconds) / default_time;
            std::cout << std::left << std::setw(24) << result.name
                      << std::right << std::setw(16) << count
                      << std::right << std::setw(16) << result.microseconds
                      << std::setw(24) << std::fixed << std::setprecision(3) << relative_time
                      << '\n';
        }

        save_normalized_results_csv(results, filepath, default_time);
        std::cout << "\nDefault generator: " << gsl_rng_default->name << '\n';
        std::cout << "Saved results to " << filepath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
