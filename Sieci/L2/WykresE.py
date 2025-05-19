import pandas as pd
import matplotlib.pyplot as plt

# Read CSV and group by Modifier to compute mean
data = pd.read_csv('Results_E.csv')
grouped = data.groupby('Modifier')['pr_result'].mean().reset_index()

# Plot just the averages
plt.plot(grouped['Modifier'], grouped['pr_result'], 
        linestyle='-', color='blue', label='Average PR Result')

# Customize the plot
plt.xlabel('Number of additional edges')
plt.ylabel('Average PR Result')
plt.title('Average PR Results by Modifier')
plt.legend()
plt.grid(True, linestyle='--', alpha=0.6)

# Save and close
plt.savefig("Results_E_Averages.png", dpi=300, bbox_inches='tight')
plt.close()