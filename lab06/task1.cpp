#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "model_isinga.h"
#include "utils.h"

namespace {

struct ExperimentSpec {
    int lattice_size;
    int energy_min;
    int energy_max;
    int energy_step;
};

struct ResultPoint {
    int lattice_size;
    int target_energy;
    double temperature;
    double mean_magnetization;
    double mean_system_energy;
    double mean_demon_energy;
};

std::vector<ExperimentSpec> build_experiments() {
    return {
        {10, -184, -24, 8},
        {20, -768, -32, 32},
        {40, -3072, -128, 128},
    };
}

std::vector<ResultPoint> run_experiments(int equilibration_steps, int averaging_steps) {
    std::vector<ResultPoint> results;

    for (const auto& experiment : build_experiments()) {
        for (int energy = experiment.energy_min; energy <= experiment.energy_max; energy += experiment.energy_step) {
            ModelIsinga model(experiment.lattice_size, energy);
            model.doprowadzenie_do_stanu_rownowagi(equilibration_steps);
            model.zliczanie_srednich(averaging_steps);

            results.push_back({
                experiment.lattice_size,
                energy,
                model.podaj_temperature(),
                model.podaj_srednia_magnetyzacje(),
                model.podaj_srednia_energie_ukladu(),
                model.podaj_srednia_energie_duszka(),
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

    out << "lattice_size,series,target_energy,temperature,mean_magnetization,mean_system_energy,mean_demon_energy\n";
    for (const auto& result : results) {
        ising::write_lattice_series_prefix(out, result.lattice_size);
        out << result.target_energy << ","
            << result.temperature << ","
            << result.mean_magnetization << ","
            << result.mean_system_energy << ","
            << result.mean_demon_energy << "\n";
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
