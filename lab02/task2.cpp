#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl_cdf.h>

namespace {

constexpr double kAlpha = 0.05;

struct ChiSquareResult {
    std::string generator_name;
    std::size_t bins;
    double statistic;
    double p_value;
    bool reject_null_hypothesis;
};

std::vector<std::size_t> count_bins(const std::vector<double>& numbers, std::size_t bins) {
    if (bins == 0) {
        throw std::invalid_argument("The number of bins must be positive.");
    }

    std::vector<std::size_t> counts(bins, 0);
    for (double number : numbers) {
        std::size_t index = static_cast<std::size_t>(number * bins);
        if (index >= bins) {
            index = bins - 1;
        }
        ++counts[index];
    }

    return counts;
}

double chi_square_uniform_statistic(const std::vector<std::size_t>& counts) {
    if (counts.empty()) {
        throw std::invalid_argument("At least one bin is required.");
    }

    std::size_t sample_count = 0;
    for (std::size_t count : counts) {
        sample_count += count;
    }

    if (sample_count == 0) {
        throw std::invalid_argument("At least one observation is required.");
    }

    double squares_sum = 0.0;
    for (std::size_t count : counts) {
        squares_sum += static_cast<double>(count) * static_cast<double>(count);
    }

    return (static_cast<double>(counts.size()) / static_cast<double>(sample_count)) * squares_sum
           - static_cast<double>(sample_count);
}

std::vector<ChiSquareResult> evaluate_uniformity(gsl_rng* generator,
                                                 const std::string& generator_name,
                                                 std::size_t sample_count,
                                                 const std::vector<std::size_t>& bins_values,
                                                 double alpha) {
    const auto numbers = generate_random_numbers(generator, sample_count);

    std::vector<ChiSquareResult> results;
    results.reserve(bins_values.size());

    for (std::size_t bins : bins_values) {
        const auto counts = count_bins(numbers, bins);
        const double statistic = chi_square_uniform_statistic(counts);
        const double degrees_of_freedom = static_cast<double>(bins - 1);
        const double p_value = gsl_cdf_chisq_Q(statistic, degrees_of_freedom);

        results.push_back({generator_name, bins, statistic, p_value, p_value < alpha});
    }

    return results;
}

void save_chi_square_results(const std::vector<ChiSquareResult>& results, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    out << "generator,bins,chi_square,p_value,reject_h0\n";
    for (const auto& result : results) {
        out << result.generator_name << ","
            << result.bins << ","
            << result.statistic << ","
            << result.p_value << ","
            << (result.reject_null_hypothesis ? "true" : "false") << "\n";
    }
}

void print_results(const std::vector<ChiSquareResult>& results) {
    for (const auto& result : results) {
        std::cout << result.generator_name
                  << ", k=" << result.bins
                  << ", chi^2=" << result.statistic
                  << ", p=" << result.p_value
                  << ", reject_h0=" << (result.reject_null_hypothesis ? "true" : "false")
                  << "\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <count> <seed> <output_csv>\n";
            return 1;
        }

        const std::size_t sample_count = std::stoull(argv[1]);
        const unsigned long seed = std::stoul(argv[2]);
        const std::string output_csv = argv[3];
        const std::vector<std::size_t> bins_values = {11, 51, 101};

        const auto good_generator = make_gsl_generator(GslGeneratorKind::Ranlux, seed);
        const auto bad_generator = make_gsl_generator(GslGeneratorKind::Randu, seed);

        auto good_results = evaluate_uniformity(
            good_generator.get(), "ranlux", sample_count, bins_values, kAlpha);
        auto bad_results = evaluate_uniformity(
            bad_generator.get(), "randu", sample_count, bins_values, kAlpha);

        std::vector<ChiSquareResult> all_results;
        all_results.reserve(good_results.size() + bad_results.size());
        all_results.insert(all_results.end(), good_results.begin(), good_results.end());
        all_results.insert(all_results.end(), bad_results.begin(), bad_results.end());

        save_chi_square_results(all_results, output_csv);
        print_results(all_results);
        std::cout << "Saved results to " << output_csv << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
