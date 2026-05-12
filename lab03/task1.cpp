#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils.hpp"

namespace {

constexpr std::size_t kUniformTerms = 12;
constexpr double kShift = 6.0;

std::vector<double> generate_approx_normal_values(gsl_rng* generator, std::size_t count) {
    std::vector<double> values;
    values.reserve(count);

    for (std::size_t sample = 0; sample < count; ++sample) {
        double sum = 0.0;
        for (std::size_t term = 0; term < kUniformTerms; ++term) {
            sum += gsl_rng_uniform(generator);
        }
        values.push_back(sum - kShift);
    }

    return values;
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <count> <seed> <filepath>\n";
            return 1;
        }

        const std::size_t count = std::stoull(argv[1]);
        const unsigned long seed = std::stoul(argv[2]);
        const std::string filepath = argv[3];

        const auto generator = make_generator(seed);
        const auto values = generate_approx_normal_values(generator.get(), count);
        save_values(values, filepath);

        std::cout << "Saved results to " << filepath << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
