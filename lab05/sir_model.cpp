#include "sir_model.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>

namespace lab05 {

Rng::Rng(int side, std::uint64_t seed)
    : generator_(seed), coordinate_(0, side - 1), unit_(0.0, 1.0) {}

Coordinates Rng::coordinates() {
    return {coordinate_(generator_), coordinate_(generator_)};
}

double Rng::unit() {
    return unit_(generator_);
}

void Rng::reseed(std::uint64_t seed) {
    generator_.seed(seed);
}

Population::Population(int side, std::uint64_t seed) : side_(side), rng_(side, seed) {
    reset();
}

void Population::reset() {
    people_.assign(side_ * side_, State::Susceptible);
    infectious_.clear();
}

long Population::size() const {
    return static_cast<long>(people_.size());
}

void Population::seed_epidemic(const Virus& virus, long initial_infectious, long initial_vaccinated) {
    virus_ = virus;

    for (long i = 0; i < initial_vaccinated; ++i) {
        const Coordinates point = random_susceptible();
        set_state(point, State::Vaccinated);
    }

    for (long i = 0; i < initial_infectious; ++i) {
        const Coordinates point = random_susceptible();
        set_state(point, State::Infectious);
        infectious_.push_back(point);
    }
}

DayStats Population::stats(int day) const {
    long susceptible = 0;
    long infectious = 0;
    long recovered = 0;
    long immune = 0;

    for (const State state_value : people_) {
        if (state_value == State::Susceptible) {
            ++susceptible;
        } else if (state_value == State::Infectious) {
            ++infectious;
        } else {
            ++immune;
            if (state_value == State::Recovered) {
                ++recovered;
            }
        }
    }

    return {day, susceptible, infectious, recovered, immune};
}

void Population::next_day() {
    std::vector<Coordinates> newly_infected;

    for (const Coordinates infectious : infectious_) {
        for (const Coordinates neighbor : susceptible_neighbors(infectious)) {
            if (rng_.unit() < virus_.beta) {
                set_state(neighbor, State::Infectious);
                newly_infected.push_back(neighbor);
            }
        }
    }

    std::vector<Coordinates> still_infectious;
    still_infectious.reserve(infectious_.size() + newly_infected.size());
    for (const Coordinates point : infectious_) {
        if (rng_.unit() < virus_.gamma) {
            set_state(point, State::Recovered);
        } else {
            still_infectious.push_back(point);
        }
    }

    still_infectious.insert(still_infectious.end(), newly_infected.begin(), newly_infected.end());
    infectious_ = std::move(still_infectious);
}

void Population::reseed(std::uint64_t seed) {
    rng_.reseed(seed);
}

void Population::save_map(const std::string& filepath) const {
    std::ofstream out(filepath);

    for (int x = 0; x < side_; ++x) {
        for (int y = 0; y < side_; ++y) {
            out << static_cast<int>(state({x, y}));
            if (y + 1 < side_) {
                out << '\t';
            }
        }
        out << '\n';
    }
}

int Population::index(Coordinates point) const {
    return side_ * point.x + point.y;
}

bool Population::on_map(Coordinates point) const {
    return point.x >= 0 && point.y >= 0 && point.x < side_ && point.y < side_;
}

State Population::state(Coordinates point) const {
    return people_[index(point)];
}

void Population::set_state(Coordinates point, State state_value) {
    people_[index(point)] = state_value;
}

Coordinates Population::random_susceptible() {
    Coordinates point;
    do {
        point = rng_.coordinates();
    } while (state(point) != State::Susceptible);
    return point;
}

std::vector<Coordinates> Population::susceptible_neighbors(Coordinates point) const {
    const std::vector<Coordinates> candidates = {
        {point.x - 1, point.y},
        {point.x + 1, point.y},
        {point.x, point.y - 1},
        {point.x, point.y + 1},
    };

    std::vector<Coordinates> neighbors;
    for (const Coordinates neighbor : candidates) {
        if (on_map(neighbor) && state(neighbor) == State::Susceptible) {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

std::vector<DayStats> run_experiment(
    int side,
    const Virus& virus,
    long initial_infectious,
    long initial_vaccinated,
    int days,
    std::uint64_t seed,
    const std::filesystem::path& map_dir,
    bool save_maps) {
    Population population(side, seed);
    population.seed_epidemic(virus, initial_infectious, initial_vaccinated);

    std::vector<DayStats> stats;
    stats.reserve(days);

    for (int day = 0; day < days; ++day) {
        stats.push_back(population.stats(day));

        if (save_maps && (day == 0 || day == 28 || day == 56 || day == 140)) {
            population.save_map((map_dir / ("map_day_" + std::to_string(day) + ".txt")).string());
        }

        population.next_day();
    }

    return stats;
}

ForecastSample analyze_forecast(int experiment, const std::vector<DayStats>& stats) {
    int peak_day = 0;
    long peak_infected = stats[0].infectious;
    for (std::size_t i = 1; i < stats.size(); ++i) {
        if (stats[i].infectious > peak_infected) {
            peak_infected = stats[i].infectious;
            peak_day = static_cast<int>(i);
        }
    }

    int inflection_day = 1;
    long max_daily_increase = stats[1].infectious - stats[0].infectious;
    const int last_growth_day = std::max(1, peak_day);
    for (int i = 2; i <= last_growth_day; ++i) {
        const long daily_increase = stats[i].infectious - stats[i - 1].infectious;
        if (daily_increase > max_daily_increase) {
            max_daily_increase = daily_increase;
            inflection_day = i;
        }
    }

    const DayStats& inflection = stats[inflection_day];
    const DayStats& final = stats.back();
    const long total_ever_infected = final.recovered + final.infectious;
    const long population_size = final.susceptible + final.immune + final.infectious;
    const long initially_non_vaccinated = final.susceptible + total_ever_infected;

    return {
        experiment,
        inflection_day,
        inflection.infectious,
        inflection.susceptible,
        inflection.recovered,
        max_daily_increase,
        peak_day,
        peak_day - inflection_day,
        peak_infected,
        peak_infected - inflection.infectious,
        static_cast<double>(peak_infected) / static_cast<double>(inflection.infectious),
        final.susceptible,
        final.infectious,
        final.recovered,
        total_ever_infected,
        100.0 * static_cast<double>(total_ever_infected) / static_cast<double>(population_size),
        100.0 * static_cast<double>(total_ever_infected) / static_cast<double>(initially_non_vaccinated),
    };
}

ObservedCase run_observed_case(
    int side,
    const Virus& virus,
    long initial_infectious,
    long initial_vaccinated,
    int days,
    std::uint64_t seed,
    const std::filesystem::path& map_dir,
    bool save_maps) {
    Population population(side, seed);
    population.seed_epidemic(virus, initial_infectious, initial_vaccinated);

    std::vector<DayStats> stats;
    std::vector<Population> snapshots;
    stats.reserve(days);
    snapshots.reserve(days);

    for (int day = 0; day < days; ++day) {
        stats.push_back(population.stats(day));
        snapshots.push_back(population);

        if (save_maps && (day == 0 || day == 28 || day == 56 || day == 140)) {
            population.save_map((map_dir / ("map_day_" + std::to_string(day) + ".txt")).string());
        }

        population.next_day();
    }

    ForecastSample actual_outcome = analyze_forecast(1, stats);
    return {stats, snapshots[actual_outcome.inflection_day], actual_outcome};
}

std::vector<DayStats> continue_experiment(Population population, int start_day, int end_day, std::uint64_t seed) {
    population.reseed(seed);

    std::vector<DayStats> stats;
    stats.reserve(end_day - start_day + 1);
    for (int day = start_day; day < end_day; ++day) {
        stats.push_back(population.stats(day));
        population.next_day();
    }

    return stats;
}

ForecastSample analyze_conditional_forecast(
    int experiment,
    int observation_day,
    long max_daily_increase,
    const std::vector<DayStats>& stats) {
    int peak_day = stats[0].day;
    long peak_infected = stats[0].infectious;
    for (const DayStats& row : stats) {
        if (row.infectious > peak_infected) {
            peak_day = row.day;
            peak_infected = row.infectious;
        }
    }

    const DayStats& observed = stats.front();
    const DayStats& final = stats.back();
    const long total_ever_infected = final.recovered + final.infectious;
    const long population_size = final.susceptible + final.immune + final.infectious;
    const long initially_non_vaccinated = final.susceptible + total_ever_infected;

    return {
        experiment,
        observation_day,
        observed.infectious,
        observed.susceptible,
        observed.recovered,
        max_daily_increase,
        peak_day,
        peak_day - observation_day,
        peak_infected,
        peak_infected - observed.infectious,
        static_cast<double>(peak_infected) / static_cast<double>(observed.infectious),
        final.susceptible,
        final.infectious,
        final.recovered,
        total_ever_infected,
        100.0 * static_cast<double>(total_ever_infected) / static_cast<double>(population_size),
        100.0 * static_cast<double>(total_ever_infected) / static_cast<double>(initially_non_vaccinated),
    };
}

double quantile(std::vector<double> values, double probability) {
    std::sort(values.begin(), values.end());
    const double position = probability * static_cast<double>(values.size() - 1);
    const auto lower = static_cast<std::size_t>(std::floor(position));
    const auto upper = static_cast<std::size_t>(std::ceil(position));
    const double weight = position - static_cast<double>(lower);
    return values[lower] * (1.0 - weight) + values[upper] * weight;
}

DistributionSummary summarize(const std::string& metric, const std::vector<double>& values) {
    const double mean = std::accumulate(values.begin(), values.end(), 0.0) / static_cast<double>(values.size());
    double sum_squared_deviations = 0.0;
    for (const double value : values) {
        const double deviation = value - mean;
        sum_squared_deviations += deviation * deviation;
    }

    auto minmax = std::minmax_element(values.begin(), values.end());
    return {
        metric,
        mean,
        std::sqrt(sum_squared_deviations / static_cast<double>(values.size() - 1)),
        quantile(values, 0.025),
        quantile(values, 0.5),
        quantile(values, 0.975),
        *minmax.first,
        *minmax.second,
    };
}

void save_trajectories(const std::vector<std::vector<DayStats>>& experiments, const std::string& filepath) {
    std::ofstream out(filepath);

    out << "experiment,series,day,susceptible,infectious,recovered,immune\n";
    for (std::size_t experiment = 0; experiment < experiments.size(); ++experiment) {
        const std::string series = "experiment_" + std::to_string(experiment + 1);
        for (const DayStats& row : experiments[experiment]) {
            out << experiment + 1 << ','
                << series << ','
                << row.day << ','
                << row.susceptible << ','
                << row.infectious << ','
                << row.recovered << ','
                << row.immune << '\n';
        }
    }
}

void save_forecast_samples(const std::vector<ForecastSample>& samples, const std::string& filepath) {
    std::ofstream out(filepath);

    out << "experiment,inflection_day,infected_at_inflection,susceptible_at_inflection,"
        << "recovered_at_inflection,max_daily_increase,peak_day,days_until_peak,peak_infected,"
        << "remaining_growth,peak_to_inflection_ratio,final_susceptible,final_infectious,"
        << "final_recovered,total_ever_infected,attack_fraction_percent,"
        << "attack_fraction_non_vaccinated_percent\n";
    for (const ForecastSample& sample : samples) {
        out << sample.experiment << ','
            << sample.inflection_day << ','
            << sample.infected_at_inflection << ','
            << sample.susceptible_at_inflection << ','
            << sample.recovered_at_inflection << ','
            << sample.max_daily_increase << ','
            << sample.peak_day << ','
            << sample.days_until_peak << ','
            << sample.peak_infected << ','
            << sample.remaining_growth << ','
            << sample.peak_to_inflection_ratio << ','
            << sample.final_susceptible << ','
            << sample.final_infectious << ','
            << sample.final_recovered << ','
            << sample.total_ever_infected << ','
            << sample.attack_fraction_percent << ','
            << sample.attack_fraction_non_vaccinated_percent << '\n';
    }
}

void save_summary(const std::vector<DistributionSummary>& summaries, const std::string& filepath) {
    std::ofstream out(filepath);

    out << "metric,mean,stddev,q025,median,q975,min,max\n";
    for (const DistributionSummary& summary : summaries) {
        out << summary.metric << ','
            << summary.mean << ','
            << summary.stddev << ','
            << summary.q025 << ','
            << summary.median << ','
            << summary.q975 << ','
            << summary.min << ','
            << summary.max << '\n';
    }
}

}  // namespace lab05
