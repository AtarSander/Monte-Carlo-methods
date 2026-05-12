import argparse
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


def read_pairs(filepath):
    x = []
    y = []

    with open(filepath, newline="") as file:
        reader = csv.reader(file)
        for row in reader:
            if len(row) < 2:
                continue

            try:
                x.append(float(row[0]))
                y.append(float(row[1]))
            except ValueError:
                continue

    return x, y


def read_column(filepath, category_column, value_column, sort=False):
    labels = []
    values = []

    with open(filepath, newline="") as file:
        reader = csv.DictReader(file)
        if sort:
            reader = sorted(reader, key=lambda row: float(row[value_column]))
        for row in reader:
            labels.append(row[category_column])
            values.append(float(row[value_column]))

    return labels, values


def read_series(filepath, x_column, y_column, error_column=None):
    x = []
    y = []
    error = []

    with open(filepath, newline="") as file:
        reader = csv.DictReader(file)
        for row in reader:
            raw_x = row.get(x_column)
            raw_y = row.get(y_column)
            if raw_x in (None, "") or raw_y in (None, ""):
                continue

            x.append(float(raw_x))
            y.append(float(raw_y))

            if error_column:
                raw_error = row.get(error_column)
                error.append(0.0 if raw_error in (None, "") else float(raw_error))

    return x, y, error


def read_grouped_series(filepath, x_column, y_column, series_column):
    series = {}

    with open(filepath, newline="") as file:
        reader = csv.DictReader(file)
        for row in reader:
            raw_x = row.get(x_column)
            raw_y = row.get(y_column)
            series_name = row.get(series_column)
            if raw_x in (None, "") or raw_y in (None, "") or series_name in (None, ""):
                continue

            if series_name not in series:
                series[series_name] = ([], [])

            x_values, y_values = series[series_name]
            x_values.append(float(raw_x))
            y_values.append(float(raw_y))

    return series


def read_histogram_values(filepath, value_column=None):
    values = []

    with open(filepath, newline="") as file:
        if value_column:
            reader = csv.DictReader(file)
            for row in reader:
                raw_value = row.get(value_column)
                if raw_value in (None, ""):
                    continue
                values.append(float(raw_value))
        else:
            reader = csv.reader(file)
            for row in reader:
                if not row:
                    continue
                values.append(float(row[0]))

    return values


def normal_pdf(x, mean=0.0, stddev=1.0):
    variance = stddev**2
    return (1.0 / np.sqrt(2.0 * np.pi * variance)) * np.exp(-((x - mean) ** 2) / (2.0 * variance))


def lab03_task3_pdf(x):
    values = np.zeros_like(x, dtype=float)
    mask = (x >= 0.0) & (x <= 2.0)
    shifted = x[mask] - 1.0
    values[mask] = (5.0 / 12.0) * (1.0 + shifted**4)
    return values


def lab03_task4_laplace_pdf(x):
    scale = 1.0
    return np.exp(-np.abs(x) / scale) / (2.0 * scale)


def lab03_task4_pareto_pdf(x):
    shape = 3.0
    scale = 1.0
    values = np.zeros_like(x, dtype=float)
    mask = x >= scale
    values[mask] = shape * np.power(scale, shape) / np.power(x[mask], shape + 1.0)
    return values


def lab03_task4_gamma_pdf(x):
    shape = 2.0
    scale = 2.0
    values = np.zeros_like(x, dtype=float)
    mask = x >= 0.0
    values[mask] = (x[mask] / (scale**2)) * np.exp(-x[mask] / scale)
    return values


def lab03_task5_cauchy_pdf(x):
    return 1.0 / (np.pi * (1.0 + x**2))


def lab03_task5_triangular_pdf(x):
    values = np.zeros_like(x, dtype=float)
    mask_left = (x >= 0.0) & (x <= 1.0)
    mask_right = (x > 1.0) & (x <= 2.0)
    values[mask_left] = x[mask_left]
    values[mask_right] = 2.0 - x[mask_right]
    return values


def lab07_task2_x1_pdf(x):
    values = np.zeros_like(x, dtype=float)
    mask = (x >= 0.0) & (x <= 4.0)
    values[mask] = 0.25
    return values


def lab07_task2_x2_pdf(x):
    values = np.zeros_like(x, dtype=float)
    mask = (x >= 5.0) & (x <= 6.0)
    values[mask] = 1.0
    return values


def lab07_task2_y_pdf(x):
    values = np.zeros_like(x, dtype=float)
    mask_left = (x >= 5.0) & (x <= 6.0)
    mask_middle = (x > 6.0) & (x <= 9.0)
    mask_right = (x > 9.0) & (x <= 10.0)
    values[mask_left] = (x[mask_left] - 5.0) / 4.0
    values[mask_middle] = 0.25
    values[mask_right] = (10.0 - x[mask_right]) / 4.0
    return values


def plot_bar(labels, values, y_label, title, save_path):
    plt.figure(figsize=(12, 6))
    plt.bar(labels, values)
    plt.title(title)
    plt.ylabel(y_label)
    plt.xticks(rotation=60, ha="right")
    plt.tight_layout()
    plt.savefig(save_path, dpi=200)
    plt.close()


def plot_scatter(x, y, x_label, y_label, title, save_path):
    plt.figure(figsize=(7, 7))
    plt.scatter(x, y, s=4, alpha=0.6)
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.tight_layout()
    plt.savefig(save_path, dpi=200)
    plt.close()


def plot_errorbar_series(
    x,
    y,
    error,
    x_label,
    y_label,
    title,
    save_path,
    log_x=False,
    reference_line=None,
):
    plt.figure(figsize=(10, 6))
    plt.errorbar(x, y, yerr=error, fmt="o-", markersize=4, linewidth=1.2, capsize=3, alpha=0.85)
    if reference_line is not None:
        plt.axhline(reference_line, color="red", linestyle="--", linewidth=1.5, label=f"reference = {reference_line:.6f}")
        plt.legend()
    if log_x:
        plt.xscale("log")
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.tight_layout()
    plt.savefig(save_path, dpi=200)
    plt.close()


def plot_multi_series(series, x_label, y_label, title, save_path, log_x=False):
    plt.figure(figsize=(10, 6))
    for label, (x_values, y_values) in series.items():
        plt.plot(x_values, y_values, marker="o", markersize=3, linewidth=1.2, label=label)
    if log_x:
        plt.xscale("log")
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.legend()
    plt.tight_layout()
    plt.savefig(save_path, dpi=200)
    plt.close()


def plot_histogram_with_empirical_distribution(values, x_label, y_label, title, save_path, bins=30):
    if not values:
        raise ValueError("Histogram plot requires at least one numeric value")

    sorted_values = np.sort(np.asarray(values, dtype=float))
    empirical_cdf = np.arange(1, len(sorted_values) + 1) / len(sorted_values)

    figure, histogram_axis = plt.subplots(figsize=(10, 6))
    histogram_axis.hist(values, bins=bins, alpha=0.7, edgecolor="black")
    histogram_axis.set_title(title)
    histogram_axis.set_xlabel(x_label)
    histogram_axis.set_ylabel(y_label)

    cdf_axis = histogram_axis.twinx()
    cdf_axis.step(sorted_values, empirical_cdf, where="post", color="red", linewidth=2)
    cdf_axis.set_ylabel("empirical CDF")
    figure.tight_layout()
    figure.savefig(save_path, dpi=200)
    plt.close(figure)


def plot_histogram_with_normal_density(
    values,
    x_label,
    y_label,
    title,
    save_path,
    bins=30,
    x_min=-4.0,
    x_max=4.0,
    mean=0.0,
    stddev=1.0,
):
    if not values:
        raise ValueError("Histogram plot requires at least one numeric value")

    clipped_values = [value for value in values if x_min <= value <= x_max]
    x = np.linspace(x_min, x_max, 500)
    y = normal_pdf(x, mean=mean, stddev=stddev)

    plt.figure(figsize=(10, 6))
    plt.hist(clipped_values, bins=bins, range=(x_min, x_max), density=True, alpha=0.7, edgecolor="black")
    plt.plot(x, y, color="red", linewidth=2, label=f"N({mean:.3g},{stddev:.3g}) density")
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.legend()
    plt.tight_layout()
    plt.savefig(save_path, dpi=200)
    plt.close()


def plot_histogram_with_custom_density(
    values, x_label, y_label, title, save_path, density_function, bins=30, x_min=0.0, x_max=1.0
):
    if not values:
        raise ValueError("Histogram plot requires at least one numeric value")

    density_functions = {
        "lab03_task3": lab03_task3_pdf,
        "lab03_task4_laplace": lab03_task4_laplace_pdf,
        "lab03_task4_pareto": lab03_task4_pareto_pdf,
        "lab03_task4_gamma": lab03_task4_gamma_pdf,
        "lab03_task5_cauchy": lab03_task5_cauchy_pdf,
        "lab03_task5_triangular": lab03_task5_triangular_pdf,
        "lab07_task2_x1": lab07_task2_x1_pdf,
        "lab07_task2_x2": lab07_task2_x2_pdf,
        "lab07_task2_y": lab07_task2_y_pdf,
    }
    if density_function not in density_functions:
        raise ValueError(f"Unknown density function: {density_function}")

    clipped_values = [value for value in values if x_min <= value <= x_max]
    x = np.linspace(x_min, x_max, 500)
    y = density_functions[density_function](x)

    plt.figure(figsize=(10, 6))
    plt.hist(clipped_values, bins=bins, range=(x_min, x_max), density=True, alpha=0.7, edgecolor="black")
    plt.plot(x, y, color="red", linewidth=2, label="target density")
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.legend()
    plt.tight_layout()
    plt.savefig(save_path, dpi=200)
    plt.close()


def main():
    argument_parser = argparse.ArgumentParser(description="Plot random number generator results")
    argument_parser.add_argument("filepath", type=Path, help="Path to the CSV file with results")
    argument_parser.add_argument("save_path", type=Path, help="Path to save the plot")
    argument_parser.add_argument(
        "--plot_type",
        choices=["bar", "scatter", "histogram", "errorbar", "histogram_normal", "histogram_custom_density", "multi_series"],
        default="scatter",
    )
    argument_parser.add_argument("--x_label", default="x_n")
    argument_parser.add_argument("--y_label", default="x_{n+k}")
    argument_parser.add_argument("--title", default="Plot")
    argument_parser.add_argument("--x_column", default="trials")
    argument_parser.add_argument("--sort", action="store_true", help="Sort values in bar plot")
    argument_parser.add_argument("--category_column", default="generator")
    argument_parser.add_argument("--series_column", default="method")
    argument_parser.add_argument("--value_column", default="time_us")
    argument_parser.add_argument("--error_column", default="stddev")
    argument_parser.add_argument("--histogram_column", default=None, help="Column name for histogram values")
    argument_parser.add_argument("--bins", type=int, default=30, help="Number of bins in histogram plot")
    argument_parser.add_argument("--x_min", type=float, default=-4.0, help="Lower range for histogram plot")
    argument_parser.add_argument("--x_max", type=float, default=4.0, help="Upper range for histogram plot")
    argument_parser.add_argument("--density_function", default=None, help="Named density function to overlay")
    argument_parser.add_argument("--log_x", action="store_true", help="Use logarithmic scale on x axis")
    argument_parser.add_argument("--reference_line", type=float, default=None, help="Optional horizontal reference line")
    argument_parser.add_argument("--normal_mean", type=float, default=0.0, help="Mean for normal density overlay")
    argument_parser.add_argument("--normal_stddev", type=float, default=1.0, help="Stddev for normal density overlay")
    args = argument_parser.parse_args()

    args.save_path.parent.mkdir(parents=True, exist_ok=True)

    if args.plot_type == "scatter":
        x, y = read_pairs(args.filepath)
        plot_scatter(x, y, args.x_label, args.y_label, args.title, args.save_path)
    elif args.plot_type == "bar":
        labels, values = read_column(args.filepath, args.category_column, args.value_column, sort=args.sort)
        plot_bar(labels, values, args.y_label, args.title, args.save_path)
    elif args.plot_type == "errorbar":
        x, y, error = read_series(args.filepath, args.x_column, args.value_column, args.error_column)
        plot_errorbar_series(
            x,
            y,
            error,
            args.x_label,
            args.y_label,
            args.title,
            args.save_path,
            log_x=args.log_x,
            reference_line=args.reference_line,
        )
    elif args.plot_type == "histogram_normal":
        values = read_histogram_values(args.filepath, args.histogram_column)
        plot_histogram_with_normal_density(
            values,
            args.x_label,
            args.y_label,
            args.title,
            args.save_path,
            bins=args.bins,
            x_min=args.x_min,
            x_max=args.x_max,
            mean=args.normal_mean,
            stddev=args.normal_stddev,
        )
    elif args.plot_type == "histogram_custom_density":
        values = read_histogram_values(args.filepath, args.histogram_column)
        plot_histogram_with_custom_density(
            values,
            args.x_label,
            args.y_label,
            args.title,
            args.save_path,
            density_function=args.density_function,
            bins=args.bins,
            x_min=args.x_min,
            x_max=args.x_max,
        )
    elif args.plot_type == "multi_series":
        series = read_grouped_series(args.filepath, args.x_column, args.value_column, args.series_column)
        plot_multi_series(
            series,
            args.x_label,
            args.y_label,
            args.title,
            args.save_path,
            log_x=args.log_x,
        )
    else:
        values = read_histogram_values(args.filepath, args.histogram_column)
        plot_histogram_with_empirical_distribution(
            values, args.x_label, args.y_label, args.title, args.save_path, bins=args.bins
        )


if __name__ == "__main__":
    main()
