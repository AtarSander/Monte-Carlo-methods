#pragma once

#include <cstdint>
#include <filesystem>
#include <random>
#include <string>
#include <vector>

namespace lab05 {

enum class State : char { Vaccinated = 0, Recovered = 1, Susceptible = 2, Infectious = 3 };

struct Coordinates {
    int x;
    int y;
};

struct Virus {
    double beta;
    double gamma;
};

struct DayStats {
    int day;
    long susceptible;
    long infectious;
    long recovered;
    long immune;
};

struct ForecastSample {
    int experiment;
    int inflection_day;
    long infected_at_inflection;
    long susceptible_at_inflection;
    long recovered_at_inflection;
    long max_daily_increase;
    int peak_day;
    int days_until_peak;
    long peak_infected;
    long remaining_growth;
    double peak_to_inflection_ratio;
    long final_susceptible;
    long final_infectious;
    long final_recovered;
    long total_ever_infected;
    double attack_fraction_percent;
    double attack_fraction_non_vaccinated_percent;
};

struct DistributionSummary {
    std::string metric;
    double mean;
    double stddev;
    double q025;
    double median;
    double q975;
    double min;
    double max;
};

class Rng {
public:
    Rng(int side, std::uint64_t seed);

    Coordinates coordinates();
    double unit();
    void reseed(std::uint64_t seed);

private:
    std::mt19937_64 generator_;
    std::uniform_int_distribution<int> coordinate_;
    std::uniform_real_distribution<double> unit_;
};

class Population {
public:
    Population(int side, std::uint64_t seed);

    void reset();
    long size() const;
    void seed_epidemic(const Virus& virus, long initial_infectious, long initial_vaccinated);
    DayStats stats(int day) const;
    void next_day();
    void reseed(std::uint64_t seed);
    void save_map(const std::string& filepath) const;

private:
    int side_;
    Rng rng_;
    Virus virus_{0.5, 0.25};
    std::vector<State> people_;
    std::vector<Coordinates> infectious_;

    int index(Coordinates point) const;
    bool on_map(Coordinates point) const;
    State state(Coordinates point) const;
    void set_state(Coordinates point, State state);
    Coordinates random_susceptible();
    std::vector<Coordinates> susceptible_neighbors(Coordinates point) const;
};

struct ObservedCase {
    std::vector<DayStats> stats;
    Population state_at_inflection;
    ForecastSample actual_outcome;
};

std::vector<DayStats> run_experiment(
    int side,
    const Virus& virus,
    long initial_infectious,
    long initial_vaccinated,
    int days,
    std::uint64_t seed,
    const std::filesystem::path& map_dir,
    bool save_maps);

ObservedCase run_observed_case(
    int side,
    const Virus& virus,
    long initial_infectious,
    long initial_vaccinated,
    int days,
    std::uint64_t seed,
    const std::filesystem::path& map_dir,
    bool save_maps);

std::vector<DayStats> continue_experiment(Population population, int start_day, int end_day, std::uint64_t seed);

ForecastSample analyze_forecast(int experiment, const std::vector<DayStats>& stats);
ForecastSample analyze_conditional_forecast(
    int experiment,
    int observation_day,
    long max_daily_increase,
    const std::vector<DayStats>& stats);

DistributionSummary summarize(const std::string& metric, const std::vector<double>& values);

void save_trajectories(const std::vector<std::vector<DayStats>>& experiments, const std::string& filepath);
void save_forecast_samples(const std::vector<ForecastSample>& samples, const std::string& filepath);
void save_summary(const std::vector<DistributionSummary>& summaries, const std::string& filepath);

}  // namespace lab05
