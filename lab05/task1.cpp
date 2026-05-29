#include "sir_model.hpp"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <vector>

namespace {

constexpr int kSide = 100;
constexpr long kInitialInfectious = 10;
constexpr double kVaccinatedFraction = 0.30;
constexpr int kBackgroundExperiments = 300;
constexpr int kConditionalForecasts = 1000;
constexpr int kDays = 220;
constexpr std::uint64_t kSeed = 132123;
constexpr std::uint64_t kObservedExperimentOffset = 6;
constexpr lab05::Virus kVirus{0.5, 0.25};

template <typename Getter>
std::vector<double> collect_metric(const std::vector<lab05::ForecastSample>& samples, Getter getter) {
    std::vector<double> values;
    values.reserve(samples.size());
    for (const auto& sample : samples) {
        values.push_back(static_cast<double>(getter(sample)));
    }
    return values;
}

std::vector<lab05::ForecastSample> run_background_ensemble(
    std::vector<std::vector<lab05::DayStats>>& trajectories,
    long initial_vaccinated,
    const std::filesystem::path& map_dir) {
    std::vector<lab05::ForecastSample> samples;
    samples.reserve(kBackgroundExperiments);
    trajectories.reserve(kBackgroundExperiments);

    for (int experiment = 0; experiment < kBackgroundExperiments; ++experiment) {
        auto stats = lab05::run_experiment(
            kSide,
            kVirus,
            kInitialInfectious,
            initial_vaccinated,
            kDays,
            kSeed + static_cast<std::uint64_t>(experiment),
            map_dir,
            false);

        samples.push_back(lab05::analyze_forecast(experiment + 1, stats));
        trajectories.push_back(std::move(stats));
    }

    return samples;
}

std::vector<lab05::ForecastSample> run_conditional_forecasts(const lab05::ObservedCase& observed_case) {
    std::vector<lab05::ForecastSample> forecasts;
    forecasts.reserve(kConditionalForecasts);

    for (int forecast = 0; forecast < kConditionalForecasts; ++forecast) {
        const auto stats = lab05::continue_experiment(
            observed_case.state_at_inflection,
            observed_case.actual_outcome.inflection_day,
            kDays,
            kSeed + 100000 + static_cast<std::uint64_t>(forecast));

        forecasts.push_back(lab05::analyze_conditional_forecast(
            forecast + 1,
            observed_case.actual_outcome.inflection_day,
            observed_case.actual_outcome.max_daily_increase,
            stats));
    }

    return forecasts;
}

std::vector<lab05::DistributionSummary> summarize_background(
    const std::vector<lab05::ForecastSample>& samples) {
    return {
        lab05::summarize("inflection_day", collect_metric(samples, [](const auto& s) { return s.inflection_day; })),
        lab05::summarize(
            "infected_at_inflection", collect_metric(samples, [](const auto& s) { return s.infected_at_inflection; })),
        lab05::summarize("peak_day", collect_metric(samples, [](const auto& s) { return s.peak_day; })),
        lab05::summarize("days_until_peak", collect_metric(samples, [](const auto& s) { return s.days_until_peak; })),
        lab05::summarize("peak_infected", collect_metric(samples, [](const auto& s) { return s.peak_infected; })),
        lab05::summarize("remaining_growth", collect_metric(samples, [](const auto& s) { return s.remaining_growth; })),
        lab05::summarize(
            "peak_to_inflection_ratio", collect_metric(samples, [](const auto& s) { return s.peak_to_inflection_ratio; })),
        lab05::summarize("final_susceptible", collect_metric(samples, [](const auto& s) { return s.final_susceptible; })),
        lab05::summarize(
            "total_ever_infected", collect_metric(samples, [](const auto& s) { return s.total_ever_infected; })),
        lab05::summarize(
            "attack_fraction_percent", collect_metric(samples, [](const auto& s) { return s.attack_fraction_percent; })),
        lab05::summarize(
            "attack_fraction_non_vaccinated_percent",
            collect_metric(samples, [](const auto& s) { return s.attack_fraction_non_vaccinated_percent; })),
    };
}

std::vector<lab05::DistributionSummary> summarize_conditional(
    const std::vector<lab05::ForecastSample>& samples) {
    return {
        lab05::summarize("peak_day", collect_metric(samples, [](const auto& s) { return s.peak_day; })),
        lab05::summarize("days_until_peak", collect_metric(samples, [](const auto& s) { return s.days_until_peak; })),
        lab05::summarize("peak_infected", collect_metric(samples, [](const auto& s) { return s.peak_infected; })),
        lab05::summarize("remaining_growth", collect_metric(samples, [](const auto& s) { return s.remaining_growth; })),
        lab05::summarize(
            "peak_to_inflection_ratio", collect_metric(samples, [](const auto& s) { return s.peak_to_inflection_ratio; })),
    };
}

void print_summary(
    const lab05::ObservedCase& observed_case,
    const std::vector<lab05::DistributionSummary>& background_summary,
    const std::vector<lab05::DistributionSummary>& conditional_summary,
    const std::filesystem::path& output_dir) {
    const auto& conditional_day = conditional_summary[0];
    const auto& conditional_peak = conditional_summary[2];
    const auto& background_day = background_summary[2];
    const auto& background_peak = background_summary[4];

    std::cout << "Observed max daily increase day: " << observed_case.actual_outcome.inflection_day << "\n";
    std::cout << "Observed infectious count then: " << observed_case.actual_outcome.infected_at_inflection << "\n";
    std::cout << "Conditional peak day median: " << conditional_day.median
              << " (95% interval: " << conditional_day.q025 << ", " << conditional_day.q975 << ")\n";
    std::cout << "Conditional peak infected median: " << conditional_peak.median
              << " (95% interval: " << conditional_peak.q025 << ", " << conditional_peak.q975 << ")\n";
    std::cout << "Background ensemble peak day median: " << background_day.median
              << " (95% interval: " << background_day.q025 << ", " << background_day.q975 << ")\n";
    std::cout << "Background ensemble peak infected median: " << background_peak.median
              << " (95% interval: " << background_peak.q025 << ", " << background_peak.q975 << ")\n";
    std::cout << "Saved results to " << output_dir << "\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::filesystem::path output_dir = argv[1];
    const std::filesystem::path map_dir = output_dir / "maps";
    std::filesystem::create_directories(output_dir);
    std::filesystem::create_directories(map_dir);

    const long initial_vaccinated = static_cast<long>(kVaccinatedFraction * kSide * kSide);
    const auto observed_case = lab05::run_observed_case(
        kSide,
        kVirus,
        kInitialInfectious,
        initial_vaccinated,
        kDays,
        kSeed + kObservedExperimentOffset,
        map_dir,
        true);

    std::vector<std::vector<lab05::DayStats>> trajectories;
    const auto background_samples = run_background_ensemble(trajectories, initial_vaccinated, map_dir);
    const auto conditional_samples = run_conditional_forecasts(observed_case);
    const auto background_summary = summarize_background(background_samples);
    const auto conditional_summary = summarize_conditional(conditional_samples);

    lab05::save_trajectories(trajectories, (output_dir / "task1_trajectories.csv").string());
    lab05::save_forecast_samples(background_samples, (output_dir / "task1_forecast_samples.csv").string());
    lab05::save_forecast_samples({observed_case.actual_outcome}, (output_dir / "task1_observed_case.csv").string());
    lab05::save_forecast_samples(conditional_samples, (output_dir / "task1_conditional_forecast_samples.csv").string());
    lab05::save_summary(background_summary, (output_dir / "task1_forecast_summary.csv").string());
    lab05::save_summary(conditional_summary, (output_dir / "task1_conditional_forecast_summary.csv").string());

    print_summary(observed_case, background_summary, conditional_summary, output_dir);

    return 0;
}
