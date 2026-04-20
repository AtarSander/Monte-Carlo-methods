import argparse
import csv
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


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


def read_column(filepath, category_column, value_column):
    labels = []
    values = []

    with open(filepath, newline="") as file:
        reader = csv.DictReader(file)
        for row in reader:
            labels.append(row[category_column])
            values.append(float(row[value_column]))

    return labels, values


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


def main():
    argument_parser = argparse.ArgumentParser(description="Plot random number generator results")
    argument_parser.add_argument("filepath", type=Path, help="Path to the CSV file with results")
    argument_parser.add_argument("save_path", type=Path, help="Path to save the plot")
    argument_parser.add_argument("--plot_type", choices=["bar", "scatter"], default="scatter")
    argument_parser.add_argument("--x_label", default="x_n")
    argument_parser.add_argument("--y_label", default="x_{n+k}")
    argument_parser.add_argument("--title", default="Plot")
    argument_parser.add_argument("--category_column", default="generator")
    argument_parser.add_argument("--value_column", default="time_us")
    args = argument_parser.parse_args()

    args.save_path.parent.mkdir(parents=True, exist_ok=True)

    if args.plot_type == "scatter":
        x, y = read_pairs(args.filepath)
        plot_scatter(x, y, args.x_label, args.y_label, args.title, args.save_path)
    else:
        labels, values = read_column(args.filepath, args.category_column, args.value_column)
        plot_bar(labels, values, args.y_label, args.title, args.save_path)


if __name__ == "__main__":
    main()
