import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Load and clean data
df = pd.read_csv("results.csv")

# Convert to numeric (handle errors)
numeric_cols = ['Array Size', 'Comparisons', 'Swaps', 'Time (microsec)']
#df[numeric_cols] = df[numeric_cols].apply(pd.to_numeric, errors='coerce')

df["Array Size"] = df["Array Size"].astype(int)
df["Comparisons"] = df["Comparisons"].astype(int)
df["Swaps"] = df["Swaps"].astype(int)
df["Time"] = df["Time (microsec)"].astype(int)

# Drop rows with missing numeric values if needed
#df = df.dropna(subset=numeric_cols)

# Compute averages
df_avg = df.groupby(["Algorithm", "Array Size"], as_index=False).mean(numeric_only=True)

def plot_metric2(metric, title, ylabel, filename):
    plt.figure(figsize=(10, 5))
    
    # Plot data for each group size
    # for size in sorted(df_avg["Group Size"].unique()):
    #     subset = df_avg[df_avg["Group Size"] == size]
    #     plt.plot(subset["Array Size"], subset[metric], linestyle='-',
    #             marker=None, label=f"Group Size {size}")

    for size in sorted(df_avg["Algorithm"].unique()):
        subset = df_avg[df_avg["Algorithm"] == size]
        plt.plot(subset["Array Size"], subset[metric], linestyle='-',
                marker=None, label=f"Algorithm {size}")

    x_values = np.array(sorted(df_avg["Array Size"].unique()))
    
    # Add reference lines
    if metric in ['Comparisons']:
        # plt.plot(x_values, 0.25*x_values*np.log2(x_values), 
        #         '--', label='0.25nlog(n)')
        plt.plot(x_values, 3*x_values*np.log2(x_values), 
                ':', label='3nlog n')
        plt.plot(x_values, 0.8*x_values*np.log2(x_values), 
                ':', label='0,8nlog n')
        plt.plot(x_values, 15*x_values*np.log2(x_values), 
                ':', label='15nlog n')       
    
    if metric in ['Swaps']:
        # plt.plot(x_values, 0.7*x_values*np.log2(x_values), 
        #         '--', label='0.7nlog(n)')
        # plt.plot(x_values, 8*x_values, 
        #         ':', label='8n')
        plt.plot(x_values, 6*x_values*np.log2(x_values), 
                ':', label='6nlog n')
        plt.plot(x_values, 0.8*x_values*np.log2(x_values), 
                ':', label='0,8nlog n')
        plt.plot(x_values, 32*x_values*np.log2(x_values), 
                ':', label='32nlog n')
    
    plt.title(title)
    plt.xlabel("Array Size(n)")
    plt.ylabel(ylabel)
    plt.legend()
    plt.grid(True)
    plt.savefig(filename, dpi=300, bbox_inches="tight")
    plt.close()

# Generate plots
plot_metric2("Comparisons", "Comparison Counts", "Comparisons", "comparisons.png")
plot_metric2("Swaps", "Swap Counts", "Swaps", "Swaps.png")
# plot_metric2("Time (microsec)", "Execution Time", "Time (μs)", "time.png")
# df_avg["Comparisons per n"] = df_avg["Comparisons"] / df_avg["Array Size"]
# df_avg["Swaps per n"] = df_avg["Swaps"] / df_avg["Array Size"]
# df_avg["Time per n"] = df_avg["Time"] / df_avg["Array Size"]
# plot_metric2("Comparisons per n", "Comparison Counts per n", "Comparisons per n", "comparisons_per_n.png")
# plot_metric2("Swaps per n", "Swap Counts per n", "Swaps per n", "Swaps_per_n.png")
# plot_metric2("Time per n", "Execution Time per n", "Time per n (μs/n)", "time_per_n.png")