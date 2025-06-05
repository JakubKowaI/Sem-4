import pandas as pd
import matplotlib.pyplot as plt

# Load CSV file
df = pd.read_csv("results_n.csv", header=None, names=["n", "comparisons"])

# Ensure numeric type for sorting and plotting
df["n"] = pd.to_numeric(df["n"], errors="coerce")
df["comparisons"] = pd.to_numeric(df["comparisons"], errors="coerce")

# Sort by operation index
#df = df.sort_values(by="n")

# Get first unique n
#n_value = df["n"].unique()[0]

# Plot
plt.figure(figsize=(10, 6))
plt.plot(df["n"], df["comparisons"]/df["n"], linestyle='-', linewidth=0.5, color='blue')

plt.title(f"Average Cost per operation")
plt.xlabel("n")
plt.ylabel("Comparisons / n")
plt.grid(True)

plt.tight_layout()
plt.savefig("comparisons_per_n_plot.png")
print("Plot saved as comparisons_per_n_plot.png")
