#include <fstream>
#include <iostream>

#include "utils.hpp"

namespace {

void save_values(const std::vector<double>& numbers, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    for (double number : numbers) {
        out << number << "\n";
    }
}

}  //namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " <count> <seed> <filepath>\n";
            return 1;
        }

        const std::size_t count = std::stoull(argv[1]);
        const unsigned long seed = std::stoul(argv[2]);
        const std::string filepath = argv[3];

        const auto generator = make_gsl_generator(GslGeneratorKind::Ranlux, seed);
        const auto numbers = generate_random_numbers(generator.get(), count);
        save_values(numbers, filepath);

        std::cout << "Saved results to " << filepath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
