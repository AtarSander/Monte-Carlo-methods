#include <iostream>
#include <random>
#include <vector>

#include "utils.hpp"


int linear_congruential_generator(int a, int c, int m, int seed) {
    return (a * seed + c) % m;
}


std::vector<int> generate_random_numbers(int a, int c, int m, int seed, int count) {
    std::vector<int> numbers;
    for (int i = 0; i < count; ++i) {
        seed = linear_congruential_generator(a, c, m, seed);
        numbers.push_back(seed);
    }
    return numbers;
}


int main(int argc, char* argv[]) {
    if (argc != 8) {
        std::cerr << "Usage: " << argv[0] << " <a> <c> <m> <seed> <count> <step> <filepath>\n";
        return 1;
    }

    int a = std::stoi(argv[1]);
    int c = std::stoi(argv[2]);
    int m = std::stoi(argv[3]);
    int seed = std::stoi(argv[4]);
    int count = std::stoi(argv[5]);
    int step = std::stoi(argv[6]);
    std::string filepath = argv[7];

    auto numbers = generate_random_numbers(a, c, m, seed, count);
    save_pairs(numbers, filepath, step);
    std::cout << "Saved results to " << filepath << "\n";

    return 0;
}
