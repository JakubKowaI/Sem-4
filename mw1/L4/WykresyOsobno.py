import pandas as pd
import matplotlib.pyplot as plt
import os

# Pliki z danymi
FILES = {
    "Increasing": "IncResults.csv",
    "Random": "RandResults.csv"
}

# Metryki do analizy
METRICS = [
    ("Comparisons", "Liczba porównań"),
    ("Pointer reads", "Odczyty wskaźników"),
    ("Pointer swaps", "Podstawienia wskaźników"),
    ("Height", "Wysokość drzewa"),
    ("Time (microsec)", "Czas operacji [nanosec]")
]

# Lista drzew
TREES = ["BST", "RBT", "ST"]

# Rysuj wykresy
def plot_tree_metrics(df, tree_type, scenario):
    df_tree = df[df["Algorithm"] == tree_type]
    if df_tree.empty:
        print(f"⚠️ Brak danych dla drzewa {tree_type} ({scenario})")
        return

    for metric, label in METRICS:
        plt.figure(figsize=(10, 6))

        for op, color, ls in [("insert", "tab:blue", "-"), ("remove", "tab:red", "--")]:
            df_op = df_tree[df_tree["Operation"] == op]
            if df_op.empty:
                continue

            # Grupowanie i agregacja
            mean_values = df_op.groupby("n")[metric].mean()
            max_values = df_op.groupby("n")[metric].max()

            # Rysowanie
            mean_values.plot(label=f"{op} (średnia)", linestyle=ls, color=color)
            max_values.plot(label=f"{op} (maksimum)", linestyle=ls, color=color, marker='x')

        plt.title(f"{label} — {tree_type} — {scenario}")
        plt.xlabel("n")
        plt.ylabel(label)
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        filename = f"{tree_type}_{metric.replace(' ', '_')}_{scenario}.png"
        plt.savefig(filename, dpi=300)
        plt.close()
        print(f"✅ Zapisano: {filename}")

# Główna pętla
for scenario, filename in FILES.items():
    if not os.path.exists(filename):
        print(f"❌ Plik nie istnieje: {filename}")
        continue

    df = pd.read_csv(filename, on_bad_lines="skip")
    for tree in TREES:
        plot_tree_metrics(df, tree, scenario)
