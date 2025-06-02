import pandas as pd
import matplotlib.pyplot as plt
import itertools
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
    ("Time (microsec)", "Czas operacji [nanosec]")
]

# Unikalne style: kolory, markery i linie
COLORS = ['tab:blue', 'tab:orange', 'tab:green', 'tab:red', 'tab:purple', 'tab:brown']
MARKERS = ['o', 's', 'D', '^', 'v', 'X']
LINESTYLES = ['-', '--']

# Przygotowanie unikalnych stylów
def get_styles(algos_ops):
    styles = {}
    combos = list(itertools.product(COLORS, MARKERS, LINESTYLES))
    for idx, key in enumerate(algos_ops):
        styles[key] = combos[idx % len(combos)]
    return styles

# Rysowanie wykresów
def plot_metric(df, scenario, metric, label):
    plt.figure(figsize=(10, 6))
    means = df.groupby(["n", "Algorithm", "Operation"]).mean(numeric_only=True).reset_index()

    algos_ops = sorted(means[["Algorithm", "Operation"]].drop_duplicates().itertuples(index=False, name=None))
    #styles = get_styles(algos_ops)

    for algo, op in algos_ops:
        subset = means[(means["Algorithm"] == algo) & (means["Operation"] == op)]
        if subset.empty:
            continue
        #color, marker, linestyle = styles[(algo, op)]
        if algo == "BST":
            color='tab:green'
            linestyle='-.'
        elif algo == "RBT":
            color = 'tab:red'
            linestyle='--'
        elif algo == "ST":
            color = 'tab:blue'
            linestyle=':'

        if op not in {"insert", "remove"}:
            continue  # pomiń inne operacje

        if op == "insert":
            marker= 'o'
        elif op == "remove":
            marker = 'X'
        

        #linestyle=':'

        plt.plot(
            subset["n"],
            subset[metric],
            label=f"{algo} - {op}",
            color=color,
            linestyle=linestyle,
            marker=marker,
            markersize=6,
            linewidth=2
        )

    plt.title(f"{label} — {scenario}")
    plt.xlabel("n")
    plt.ylabel(label)
    plt.legend(ncol=2)
    plt.grid(True)
    plt.tight_layout()

    filename = f"{metric.replace(' ', '_')}_AllTrees_{scenario}.png"
    plt.savefig(filename, dpi=300)
    plt.close()
    print(f"✅ Zapisano: {filename}")

# Przetwarzanie plików
for scenario, filename in FILES.items():
    if not os.path.exists(filename):
        print(f"⚠️ Plik nie znaleziony: {filename}")
        continue

    df = pd.read_csv(filename)
    for metric, label in METRICS:
        plot_metric(df, scenario, metric, label)
