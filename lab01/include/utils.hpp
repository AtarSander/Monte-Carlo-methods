#ifndef LAB01_UTILS_HPP
#define LAB01_UTILS_HPP

#include <chrono>
#include <string>
#include <vector>

struct BenchmarkResult {
    std::string name;
    long long microseconds;
};

using Clock = std::chrono::steady_clock;
using Moment = Clock::time_point;

Moment now();
long long us_between(Moment start, Moment end);
long long shortest_time(const std::vector<BenchmarkResult>& results);

void save_pairs(const std::vector<int>& numbers, const std::string& filename, int step = 1);
void save_pairs(const std::vector<double>& numbers, const std::string& filename, int step = 1);
void save_results_csv(const std::vector<BenchmarkResult>& results, const std::string& filename);
void save_normalized_results_csv(const std::vector<BenchmarkResult>& results,
                                 const std::string& filename,
                                 long long reference_microseconds);

#endif
