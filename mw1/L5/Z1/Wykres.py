import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("mst_times.csv")
plt.plot(data["n"], data["kruskal_time"], label="Kruskal")
plt.plot(data["n"], data["prim_time"], label="Prim")
plt.xlabel("Liczba wierzchołków")
plt.ylabel("Czas (ms)")
plt.title("Porównanie czasów działania MST: Kruskal vs Prim")
plt.legend()
plt.grid(True)
plt.savefig("MST_times.png", dpi=300, bbox_inches="tight")
plt.close()
