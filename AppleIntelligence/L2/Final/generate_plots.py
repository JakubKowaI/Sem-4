import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results.csv")

h1_data = df[df['heurystyka'] == 'h1'].reset_index(drop=True)
h2_data = df[df['heurystyka'] == 'h2'].reset_index(drop=True)

plt.figure(figsize=(10, 6))
plt.plot(h1_data['dlugosc_rozwiazania'], h1_data['liczba_stanow'], 'o', label='h1 - Manhattan')
#plt.plot(h2_data.index, h2_data['liczba_stanow'], marker='s', label='h2 - Hamming')
plt.title('Liczba odwiedzonych stanów / Dlugosc rozwiazania')
plt.xlabel('dlugosc_rozwiazania')
plt.ylabel('Liczba odwiedzonych stanów')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('Manhattan.png')
plt.close()

plt.figure(figsize=(10, 6))
#plt.plot(h1_data.index, h1_data['dlugosc_rozwiazania'], marker='o', label='h1 - Manhattan')
plt.plot(h2_data['dlugosc_rozwiazania'], h2_data['liczba_stanow'], 'o', label='h2 - Hamming')
plt.title('Liczba odwiedzonych stanów / Dlugosc rozwiazania')
plt.xlabel('dlugosc_rozwiazania')
plt.ylabel('Liczba odwiedzonych stanów')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('Hamming.png')
plt.close()

plt.figure(figsize=(10, 6))
plt.plot(h1_data['dlugosc_rozwiazania'], h1_data['liczba_stanow'], 'o', label='h1 - Manhattan')
plt.plot(h2_data['dlugosc_rozwiazania'], h2_data['liczba_stanow'], 'o', label='h2 - Hamming')
plt.title('Stosunek liczby odwiedzonych stanów obu heurystyk')
plt.xlabel('dlugosc_rozwiazania')
plt.ylabel('Liczba odwiedzonych stanów')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('Stosunek.png')
plt.close()

print("Wykresy zapisano do plików PNG.")
