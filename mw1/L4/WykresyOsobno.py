import pandas as pd
import matplotlib.pyplot as plt
import os

# Nazwy plików CSV
FILES = {
    "Increasing": "IncResults.csv",
    "Random": "RandResults.csv"
}

# Miary do wykresów: (kolumna, etykieta)
METRICS = [
    ("Comparisons", "Liczba porównań"),
    ("Pointer reads", "Odczyty wskaźników"),
    ("Pointer swaps", "Podstawienia wskaźników"),
    ("Height", "Wysokość drzewa"),
    ("Time (microsec)", "Czas operacji [μs]")
]

# Styl graficzny dla operacji
OPERATION_STYLE = {
    "insert": {"marker": "o", "linestyle": "-", "label": "insert"},
    "remove": {"marker": "X", "linestyle": "--", "label": "remove"}
}

# Kolor dla typu drzewa
ALGO_COLOR = {
    "BST": "tab:green",
    "RBT": "tab:red",
    "ST": "tab:blue"
}

# Rysowanie wykresów dla jednego algorytmu (BST/RBT/ST)
def plot_metric_per_algorithm(df, scenario, algorithm, metric, label):
    plt.figure(figsize=(10, 6))
    means = df[df["Algorithm"] == algorithm].groupby(["n", "Operation"]).mean(numeric_only=True).reset_index()

    for op in ["insert", "remove"]:
        subset = means[means["Operation"] == op]
        if subset.empty:
            continue

        # Styl
        style = OPERATION_STYLE[op]
        color = ALGO_COLOR.get(algorithm, "black")

        # Średnia i maksimum
        avg_val = subset[metric].mean()
        max_val = subset[metric].max()
        print(f"📊 {scenario} | {algorithm} | {label} | {op} — Średnia: {avg_val:.2f}, Maksimum: {max_val:.2f}")

        # Wykres
        plt.plot(
            subset["n"],
            subset[metric],
            label=style["label"],
            color=color,
            linestyle=style["linestyle"],
            marker=style["marker"],
            markersize=6,
            linewidth=2
        )

    plt.title(f"{label} — {algorithm} — {scenario}")
    plt.xlabel("n")
    plt.ylabel(label)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

    filename = f"{metric.replace(' ', '_')}_{algorithm}_{scenario}.png"
    plt.savefig(filename, dpi=300)
    plt.close()
    print(f"✅ Zapisano: {filename}")

# Przetwarzanie plików
for scenario, filename in FILES.items():
    if not os.path.exists(filename):
        print(f"⚠️ Plik nie znaleziony: {filename}")
        continue

    df = pd.read_csv(filename)
    algorithms = df["Algorithm"].unique()

    for algorithm in algorithms:
        for metric, label in METRICS:
            plot_metric_per_algorithm(df, scenario, algorithm, metric, label)
