#include "utils.h"

#include <ostream>
#include <stdexcept>

namespace ising {

void validate_simulation_steps(int equilibration_steps, int averaging_steps) {
    if (equilibration_steps < kMinimumEquilibrationSteps) {
        throw std::invalid_argument("Equilibration steps must be greater than 1000.");
    }
    if (averaging_steps < kMinimumAveragingSteps) {
        throw std::invalid_argument("Averaging steps must be greater than 1000.");
    }
}

std::string make_lattice_series_label(int lattice_size) {
    return "L=" + std::to_string(lattice_size);
}

void write_lattice_series_prefix(std::ostream& out, int lattice_size) {
    out << lattice_size << "," << make_lattice_series_label(lattice_size) << ",";
}

}  // namespace ising
