import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#import seaborn as sns

k=1

# Load the data from CSV
#df = pd.read_csv("plots/HighN/results/results50K"+str(k)+".csv")
df = pd.read_csv("plots/Z2/results/results.csv")
#df = pd.read_csv("results.csv")

# Ensure correct data types
df["Array Size"] = df["Array Size"].astype(int)
df["Comparisons"] = df["Comparisons"].astype(int)
df["Swaps"] = df["Swaps"].astype(int)
df["Time"] = df["Time (microsec)"].astype(int)

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
    y_values = 0.4*x_values * np.log2(x_values)
    #y_values = 0.25*(x_values*x_values)

    y_values2= 0.1*x_values * np.log2(x_values)

    # Plot x log2(x) as a reference
    plt.plot(x_values, y_values, linestyle="--", color="black", label="0,4*nlog2(n)")

    plt.plot(x_values, y_values2, linestyle="--", color="black", label="0,1*nlog2(n)")

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
#plot_metric("Comparisons", "Average Number of Comparisons vs Array Size", "Average Comparisons", "comparisons_avg.png")
#plot_metric("Swaps", "Average Number of Swaps vs Array Size", "Average Swaps", "swaps_avg.png")
plot_metric2("Time", "Average Time of Solving vs Array Size", "Average time (μs)", "time_avg.png")
#df_avg["Comparisons per n"] = df_avg["Comparisons"] / df_avg["Array Size"]
#df_avg["Swaps per n"] = df_avg["Swaps"] / df_avg["Array Size"]
df_avg["Time per n"] = df_avg["Time"] / df_avg["Array Size"]
#plot_metric2("Comparisons per n", "Comparisons per n", "Comparisons / n", "comparisons_per_n.png")
#plot_metric2("Swaps per n", "Swaps per n", "Swaps / n", "swaps_per_n.png")
plot_metric2("Time per n", "Average time per n", "Average time per n (μs/n)", "time_avg_per_n.png")
print("Plots saved: comparisons_avg.png, swaps_avg.png, comparisons_per_n.png, swaps_per_n.png")
