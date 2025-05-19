import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
data = pd.read_csv('Results_C.csv')

# Create the plot
plt.plot(data['Modifier'], data['pr_result'])

# Add basic labels
plt.xlabel('Modifier')
plt.ylabel('PR Result')
plt.title('PR Results for variable capacity (C*Modifier)')

# Show the plot
plt.savefig("Results_C.png")