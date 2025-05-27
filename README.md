# Arduino – Diagnostyka i Monitoring Obrotów Wału Korbowego

Projekt oparty na **Arduino UNO**, który umożliwia analizę pracy dwucylindrowego wału korbowego (wydrukowanego w 3D) z użyciem czujnika zbliżeniowego, pomiar temperatury, detekcję kolejności impulsów oraz wyświetlanie danych na ekranie OLED. System zawiera również przycisk sterujący oraz przekaźnik do dalszej rozbudowy.

---

## Wykorzystane komponenty

| Element                     | Opis                                                  |
|----------------------------|--------------------------------------------------------|
| Arduino Uno                | Główny mikrokontroler sterujący pracą systemu         |
| Proximity Switch LJ12A3-4-Z/BX | Detekcja znaczników na wale korbowym (16 pozycji) |
| Przekaźnik 1-kanałowy      | Do sterowania urządzeniem zewnętrznym                 |
| Wyświetlacz OLED I2C IIC 128x64 SS - D - 1306 3,3 V-5 V | Wyświetlanie temperatury, stanu pracy i RPM           |
| Termistor 100k NTC         | Pomiar temperatury                                    |
| Przycisk                   | Włączanie/wyłączanie trybu monitorowania              |

---

## Opis działania

### 1. Wykrywanie pinu
Model wału zawiera 16 znaczników (w tym jeden pusty slot jako "pin zerowy"). Czujnik zbliżeniowy rejestruje zmiany stanu, które mikroprocesor interpretuje jako kolejne impulsy.

### 2. Ustalanie kolejności zapłonów
W celu eliminacji zakłóceń i błędów odczytu system wykonuje trzykrotną próbę detekcji tego samego pinu.

### 3. Sterowanie przekaźnikiem
Jeśli wykryty zostaje pin startowy lub końcowy (np. 5 i 9), aktywowany jest przekaźnik, który może sterować elementem wykonawczym (np. zapłonem).

### 4. Pomiar temperatury
System odczytuje temperaturę za pomocą termistora i przelicza ją przy użyciu równania Steinharta-Harta. Wynik prezentowany jest na wyświetlaczu.

### 5. Pomiar prędkości obrotowej (zakomentowane)
Obliczenia RPM są obecnie wyłączone z powodu ograniczeń pamięci Arduino UNO – planowane włączenie po zmianie sprzętu.

---

## Kluczowe funkcje w kodzie

- `ustalPrawdziwyPin()` – trzykrotnie potwierdza poprawność wykrytego pinu.
- `sterowaniePrzekaznikiem()` – przełącza stan przekaźnika na podstawie pozycji wału.
- `zmierzTemperature()` – zwraca aktualną temperaturę w °C.
- `sprawdzPrzycisk()` – aktywuje lub dezaktywuje system.
- `zmianaStanu()` – wykrywa impuls z czujnika i aktualizuje licznik.
- `zmierzPredkosc()` – _(zakomentowane)_ oblicza RPM.
- `wyswietlPredkosc()` – _(zakomentowane)_ prezentuje RPM na OLEDzie.
