#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "model_isinga.h"
#include "utils.h"

namespace {

struct ResultPoint {
    int lattice_size;
    double temperature;
    double mean_magnetization;
    double mean_system_energy;
};

std::vector<int> build_lattice_sizes() {
    return {10, 20, 40};
}

std::vector<double> build_temperature_grid() {
    return {
        1.50, 1.70, 1.90,
        2.00, 2.05, 2.10, 2.15, 2.20, 2.25, 2.30, 2.35, 2.40, 2.45, 2.50, 2.55, 2.60,
        2.70, 2.80, 3.00, 3.20, 3.50, 4.00
    };
}

std::vector<ResultPoint> run_experiments(int equilibration_steps, int averaging_steps) {
    std::vector<ResultPoint> results;
    const auto lattice_sizes = build_lattice_sizes();
    const auto temperatures = build_temperature_grid();
    results.reserve(lattice_sizes.size() * temperatures.size());

    for (const auto lattice_size : lattice_sizes) {
        for (const auto temperature : temperatures) {
            ModelIsinga model(lattice_size, static_cast<float>(temperature));
            model.doprowadzenie_do_stanu_rownowagi(equilibration_steps);
            model.zliczanie_srednich(averaging_steps);

            results.push_back({
                lattice_size,
                temperature,
                model.podaj_srednia_magnetyzacje(),
                model.podaj_srednia_energie_ukladu(),
            });
        }
    }

    std::sort(results.begin(), results.end(), [](const ResultPoint& lhs, const ResultPoint& rhs) {
        if (lhs.lattice_size != rhs.lattice_size) {
            return lhs.lattice_size < rhs.lattice_size;
        }
        return lhs.temperature < rhs.temperature;
    });

    return results;
}

void save_results(const std::vector<ResultPoint>& results, const std::string& output_csv) {
    std::ofstream out(output_csv);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + output_csv);
    }

    out << "lattice_size,series,temperature,mean_magnetization,mean_system_energy\n";
    for (const auto& result : results) {
        ising::write_lattice_series_prefix(out, result.lattice_size);
        out << result.temperature << ","
            << result.mean_magnetization << ","
            << result.mean_system_energy << "\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0]
                      << " <equilibration_steps> <averaging_steps> <output_csv>\n";
            return 1;
        }

        const int equilibration_steps = std::stoi(argv[1]);
        const int averaging_steps = std::stoi(argv[2]);
        const std::string output_csv = argv[3];

        ising::validate_simulation_steps(equilibration_steps, averaging_steps);

        const auto results = run_experiments(equilibration_steps, averaging_steps);
        save_results(results, output_csv);

        std::cout << "Saved results to " << output_csv << "\n";
        std::cout << "Computed " << results.size() << " experiment points.\n";
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }

    return 0;
}
