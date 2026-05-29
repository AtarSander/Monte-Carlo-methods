#ifndef ISING_UTILS_H
#define ISING_UTILS_H

#include <iosfwd>
#include <string>

namespace ising {

constexpr int kMinimumEquilibrationSteps = 1001;
constexpr int kMinimumAveragingSteps = 1001;

void validate_simulation_steps(int equilibration_steps, int averaging_steps);
std::string make_lattice_series_label(int lattice_size);
void write_lattice_series_prefix(std::ostream& out, int lattice_size);

}  // namespace ising

#endif // ISING_UTILS_H
