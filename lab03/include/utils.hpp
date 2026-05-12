#ifndef LAB03_UTILS_HPP
#define LAB03_UTILS_HPP

#include <memory>
#include <string>
#include <vector>

#include <gsl/gsl_rng.h>

using GslRngPtr = std::unique_ptr<gsl_rng, void (*)(gsl_rng*)>;

struct TimingResult {
    std::string method;
    long long microseconds;
};

GslRngPtr make_generator(unsigned long seed);
void save_values(const std::vector<double>& values, const std::string& filepath);
void save_timing_results(const std::vector<TimingResult>& results, const std::string& filepath);

#endif
