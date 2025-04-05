import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

k=100

# Load the data from CSV
#df = pd.read_csv("resultsQSK"+str(k)+".csv")
df = pd.read_csv("results.csv")


# Ensure correct data types
df["Array Size"] = df["Array Size"].astype(int)
df["Comparisons"] = df["Comparisons"].astype(int)
df["Swaps"] = df["Swaps"].astype(int)

# Compute the average comparisons and swaps per array size for each algorithm
df_avg = df.groupby(["Algorithm", "Array Size"], as_index=False).mean()

# Function to generate line plots
def plot_metric(metric, title, ylabel, filename):
    plt.figure(figsize=(10, 5))
    
    # Plot sorting algorithm data
    for algo in df_avg["Algorithm"].unique():
        subset = df_avg[df_avg["Algorithm"] == algo]
        plt.plot(subset["Array Size"], subset[metric], marker="o", label=algo)

    # Compute x log2(x) for the given x values
    x_values = np.array(sorted(df_avg["Array Size"].unique()))
    y_values = 0.6*x_values * np.log2(x_values)

    # Plot x log2(x) as a reference
    plt.plot(x_values, y_values, linestyle="--", color="black", label="0,6*x log2(x)")

    plt.title(title)
    plt.xlabel("Array Size (n)")
    plt.ylabel(ylabel)
    plt.legend(title="Algorithm")
    plt.grid(True, linestyle="--", alpha=0.7)

    # Save the plot
    plt.savefig(filename, dpi=300, bbox_inches="tight")
    plt.close()


def plot_metric2(metric, title, ylabel, filename):
    plt.figure(figsize=(10, 5))
    
    # Plot sorting algorithm data
    for algo in df_avg["Algorithm"].unique():
        subset = df_avg[df_avg["Algorithm"] == algo]
        plt.plot(subset["Array Size"], subset[metric], marker="o", label=algo)

    # Compute x log2(x) for the given x values
    x_values = np.array(sorted(df_avg["Array Size"].unique()))
    #y_values = 0.6*x_values * np.log2(x_values)

    # Plot x log2(x) as a reference
    #plt.plot(x_values, y_values, linestyle="--", color="black", label="0,6*x log2(x)")

    plt.title(title)
    plt.xlabel("Array Size (n)")
    plt.ylabel(ylabel)
    plt.legend(title="Algorithm")
    plt.grid(True, linestyle="--", alpha=0.7)

    # Save the plot
    plt.savefig(filename, dpi=300, bbox_inches="tight")
    plt.close()

# Generate and save the required plots
plot_metric2("Comparisons", "Average Number of Comparisons vs Array Size", "Average Comparisons", "plots/Z3/comparisons_avg"+str(k)+".png")
plot_metric2("Swaps", "Average Number of Swaps vs Array Size", "Average Swaps", "plots/Z3/swaps_avg"+str(k)+".png")
df_avg["Comparisons per n"] = df_avg["Comparisons"] / df_avg["Array Size"]
df_avg["Swaps per n"] = df_avg["Swaps"] / df_avg["Array Size"]
plot_metric2("Comparisons per n", "Comparisons per n", "Comparisons / n", "plots/Z3/comparisons_per_n"+str(k)+".png")
plot_metric2("Swaps per n", "Swaps per n", "Swaps / n", "plots/Z3/swaps_per_n"+str(k)+".png")

print("Plots saved: comparisons_avg.png, swaps_avg.png, comparisons_per_n.png, swaps_per_n.png")
