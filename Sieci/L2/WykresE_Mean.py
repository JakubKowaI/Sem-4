import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the CSV file
data = pd.read_csv('Results_E.csv')

# Group by 'Modifier' and compute mean & std deviation
grouped = data.groupby('Modifier')['pr_result'].agg(['mean', 'std']).reset_index()

# Create the plot with error bars
plt.errorbar(
    grouped['Modifier'],
    grouped['mean'],
    yerr=grouped['std'],
    fmt='-o',           # Line with circle markers
    capsize=5,          # Error bar cap width
    label='PR Results ± Std Dev'
)

# Add a horizontal line for the overall mean
overall_mean = data['pr_result'].mean()
plt.axhline(
    overall_mean,
    color='red',
    linestyle='--',
    label=f'Overall Mean: {overall_mean:.4f}'
)

# Customize the plot
plt.xlabel('Number of additional edges')
plt.ylabel('PR Result')
plt.title('PR Results (Grouped by Modifier)')
plt.legend()
plt.grid(True, linestyle='--', alpha=0.6)  # Add grid for better readability

# Save the plot
plt.savefig("Results_E_grouped.png", dpi=300, bbox_inches='tight')
plt.close()