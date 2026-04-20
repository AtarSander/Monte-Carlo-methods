#include "utils.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>


Moment now() {
    return Clock::now();
}


long long us_between(Moment start, Moment end) {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}


long long shortest_time(const std::vector<BenchmarkResult>& results) {
    const auto shortest = std::min_element(
        results.begin(),
        results.end(),
        [](const BenchmarkResult& lhs, const BenchmarkResult& rhs) {
            return lhs.microseconds < rhs.microseconds;
        });

    return shortest->microseconds;
}


void save_pairs(const std::vector<int>& numbers, const std::string& filename, int step) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    for (size_t i = 0; i + step < numbers.size(); i++) {
        out << numbers[i] << ", " << numbers[i + step] << "\n";
    }
    out.close();
}


void save_pairs(const std::vector<double>& numbers, const std::string& filename, int step) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    for (size_t i = 0; i + step < numbers.size(); i++) {
        out << numbers[i] << ", " << numbers[i + step] << "\n";
    }
    out.close();
}


void save_results_csv(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    out << "generator,time_us\n";
    for (const auto& r : results) {
        out << r.name << "," << r.microseconds << "\n";
    }
}


void save_normalized_results_csv(const std::vector<BenchmarkResult>& results,
                                 const std::string& filename,
                                 long long reference_microseconds) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    out << "generator,time_us,relative_to_default\n";
    for (const auto& r : results) {
        const double relative_time = static_cast<double>(r.microseconds) / reference_microseconds;
        out << r.name << "," << r.microseconds << "," << relative_time << "\n";
    }
}
