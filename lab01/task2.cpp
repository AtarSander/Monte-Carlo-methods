#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "utils.hpp"


double wichmann_and_hill_generator(int& x, int& y, int& z) {
    x = (171 * x) % 30269;
    y = (172 * y) % 30307;
    z = (170 * z) % 30323;
    return std::fmod(x / 30269.0 + y / 30307.0 + z / 30323.0, 1.0);
}


std::vector<double> generate_random_numbers(int x, int y, int z, int count) {
    std::vector<double> numbers;
    numbers.reserve(count);

    for (int i = 0; i < count; ++i) {
        numbers.push_back(wichmann_and_hill_generator(x, y, z));
    }

    return numbers;
}


int main(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <x> <y> <z> <count> <step> <filepath>\n";
        return 1;
    }

    int x = std::stoi(argv[1]);
    int y = std::stoi(argv[2]);
    int z = std::stoi(argv[3]);
    int count = std::stoi(argv[4]);
    int step = std::stoi(argv[5]);
    std::string filepath = argv[6];

    auto numbers = generate_random_numbers(x, y, z, count);
    save_pairs(numbers, filepath, step);
    std::cout << "Saved results to " << filepath << "\n";

    return 0;
}
