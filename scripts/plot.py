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

            x.append(float(row[0]))
            y.append(float(row[1]))

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


def main():
    argument_parser = argparse.ArgumentParser(description="Plot random number generator results")
    argument_parser.add_argument("filepath", type=Path, help="Path to the CSV file with results")
    argument_parser.add_argument("save_path", type=Path, help="Path to save the plot")
    argument_parser.add_argument("--plot_type", choices=["bar", "scatter", "histogram", "errorbar"], default="scatter")
    argument_parser.add_argument("--x_label", default="x_n")
    argument_parser.add_argument("--y_label", default="x_{n+k}")
    argument_parser.add_argument("--title", default="Plot")
    argument_parser.add_argument("--x_column", default="trials")
    argument_parser.add_argument("--sort", action="store_true", help="Sort values in bar plot")
    argument_parser.add_argument("--category_column", default="generator")
    argument_parser.add_argument("--value_column", default="time_us")
    argument_parser.add_argument("--error_column", default="stddev")
    argument_parser.add_argument("--histogram_column", default=None, help="Column name for histogram values")
    argument_parser.add_argument("--bins", type=int, default=30, help="Number of bins in histogram plot")
    argument_parser.add_argument("--log_x", action="store_true", help="Use logarithmic scale on x axis")
    argument_parser.add_argument("--reference_line", type=float, default=None, help="Optional horizontal reference line")
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
    else:
        values = read_histogram_values(args.filepath, args.histogram_column)
        plot_histogram_with_empirical_distribution(
            values, args.x_label, args.y_label, args.title, args.save_path, bins=args.bins
        )


if __name__ == "__main__":
    main()
