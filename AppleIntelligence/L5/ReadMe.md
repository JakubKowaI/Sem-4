1. Opis problemu

Celem zadania było zaimplementowanie sieci neuronowej rozwiązującej problem:
Dla dwóch liczb rzeczywistych x1,x2∈[−1,1], różnych od zera, funkcja ma zwrócić:

    1, jeśli x1​ i x2​ mają ten sam znak (czyli x1⋅x2≥0),

    0 w przeciwnym przypadku.

2. Architektura sieci

Zaimplementowano dwuwarstwową sieć neuronową:

    Warstwa ukryta: 4 neurony z funkcją aktywacji: ReLU lub sigmoid (W testach użyto relu),

    Warstwa wyjściowa: 1 neuron z funkcją aktywacji: sigmoid (dla klasyfikacji binarnej),

    Algorytm: klasyczny backpropagation (propagacja wsteczna) z aktualizacją wag przy każdej próbce.

3. Przygotowanie danych

Dane generowane były losowo: 100 par liczb (x1,x2)∈[−1,1]. Odpowiedź miała być oznaczona jako 1 lub 0 na podstawie znaku iloczynu.

Przetestowano 3 warianty danych wejściowych:

    Bez normalizacji (brak przekształceń),

    Z normalizacją L1 – dzielenie przez sumę wartości wejściowych,

    Z normalizacją L2 – dzielenie przez długość wektora (normę euklidesową).

4. Eksperymenty

Dla każdej kombinacji:

    Normalizacji (None, L1, L2),

    Współczynnika uczenia learning_rate w zakresie od 0.1 do 0.5 (co 0.1),

sieć trenowano przez 5000 epok na 100 przykładach, a następnie testowano na 10% danych.
Każdy wariant powtórzono 15 razy, a wyniki zapisano do pliku results.csv.
5. Wyniki i analiza

Wyniki zapisano w formacie CSV, a następnie wykorzystano skrypt Pythona do wizualizacji zależności między średnią skutecznością (accuracy) a współczynnikiem uczenia, dla każdej metody normalizacji.
Wnioski:

    

    Wpływ normalizacji:

        L2 dawała najlepsze rezultaty.

        L1 była kiepska, czę.

        Brak normalizacji skutkował większą zmiennością wyników i wolniejszym uczeniem.

    Wpływ współczynnika uczenia:

        Przy funkcji normalizacji L2, która wydawała się najlepsza w obsłudze danych widać stosunkowo stabilny wzrost skuteczności sieci wraz z współczynnikiem uczenia.

        Dla danych nieznormalizowanych wnioski są podobne, ale z o wiele bardziej niestabilne. Dla współczynnika uczenia 0,5 sieć wyglądała trochę na przeuczoną.

    Dokładność klasyfikacji:

        Przy optymalnych parametrach sieć osiągała skuteczność powyżej 90%.

6. Wnioski końcowe

Za pomocą prostej, dwuwarstwowej sieci neuronowej z 4 neuronami ukrytymi i jedną jednostką wyjściową można skutecznie nauczyć się klasyfikować znaki dwóch liczb.
Najlepsze wyniki uzyskano stosując:

    ReLU w warstwie ukrytej,

    Normalizację L2 danych wejściowych,

    Współczynnik uczenia w zakresie 0.3–0.4.

Implementacja została zrealizowana w języku C++, a do analizy wyników i tworzenia wykresów wykorzystano skrypt w języku Python.