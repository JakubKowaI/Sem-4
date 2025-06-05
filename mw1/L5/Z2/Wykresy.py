import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
df = pd.read_csv('mst_times_and_rounds.csv')

# Create separate plots for Kruskal and Prim
def plot_metric(df, algo_prefix, title, filename):
    plt.figure(figsize=(10, 6))
    plt.plot(df['n'], df[f'{algo_prefix}_average_rounds'], label='Average rounds', marker='o')
    plt.plot(df['n'], df[f'{algo_prefix}_min_rounds'], label='Min rounds', marker='s')
    plt.plot(df['n'], df[f'{algo_prefix}_max_rounds'], label='Max rounds', marker='^')
    plt.xlabel('Number of nodes (n)')
    plt.ylabel('Number of rounds')
    plt.title(title)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()

# Generate plots
plot_metric(df, 'kruskal', 'Broadcast Rounds in Kruskal MST', 'kruskal_rounds.png')
plot_metric(df, 'prim', 'Broadcast Rounds in Prim MST', 'prim_rounds.png')

print("Plots saved as 'kruskal_rounds.png' and 'prim_rounds.png'")
