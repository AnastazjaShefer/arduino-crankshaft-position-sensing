// ================== WYSWIETLACZ OLED ==================
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SZEROKOSC_EKRANU 128
#define WYSOKOSC_EKRANU 64
#define ADRES_OLED 0x3C  // Adres I2C (najczęściej 0x3C)

Adafruit_SSD1306 wyswietlacz(SZEROKOSC_EKRANU, WYSOKOSC_EKRANU, &Wire, -1);

// ================== TERMISTOR ==================
#define PIN_TERMISTORA A0
#define REZYSTOR_SZEREGOWY 100000  // 100k
#define ODPORNOSC_NOMINALNA 100000
#define TEMPERATURA_NOMINALNA 25
#define WSPOLCZYNNIK_B 3950  // Sprawdź dane katalogowe termistora

// ================== CZUJNIK ZBLIŻENIOWY ==================
const int pinCzujnika = 7;

// ================== PRZEKAŹNIK ==================
const int pinPrzekaznika1 = 11;

// ================== PRZYCISK WL/WYL ==================
const byte pinPrzycisku = 4;

// ================== ZMIENNE GLOBALNE ==================
int poprzedniStan, liczbaSygnalow = 0;
int nrPinuWal = -1;
unsigned long sredniCzas = 0;
unsigned long czasZakonczenia = 0.0;

// ================== FUNKCJE ==================

// Funkcja do wykrywania numeru pinu na podstawie czasu reakcji
unsigned long indeksPinu() {
  unsigned long czas = 0, maxCzas = 0, czasStartu;
  int numerPinu;

  czasStartu = micros();
  while (!zmianaStanu() && digitalRead(pinPrzycisku) == LOW) {}
  for (int i = 2; i <= 15; i++) {
    czasZakonczenia = micros();
    czas = czasZakonczenia - czasStartu;
    if (czas > maxCzas) {
      maxCzas = czas;
      numerPinu = i;
    }
    czasStartu = micros();
    while (!zmianaStanu() && digitalRead(pinPrzycisku) == LOW) {}
  }
  return 16 - numerPinu + 1;
}

// Ustalanie prawdziwego pinu przez trzykrotne potwierdzenie
int ustalPrawdziwyPin() {
  int i = 0, pin1 = -1;
  while (i < 3 && digitalRead(pinPrzycisku) == LOW) {
    liczbaSygnalow = 1;
    int pin2 = indeksPinu();
    if (pin2 != pin1) {
      i = 0;
      pin1 = pin2;
      continue;
    }
    pin1 = pin2;
    i++;
  }
  return pin1;
}

// Sterowanie przekaźnikiem na podstawie pinu
void sterowaniePrzekaznikiem(int pinStart, int pinKoniec, char pinSterujacy) {
  if (nrPinuWal == pinStart) digitalWrite(pinSterujacy, LOW);
  else if (nrPinuWal == pinKoniec) digitalWrite(pinSterujacy, HIGH);
}

// Zmiana numeru pinu walki w zakresie 1–15
void zmienPinWalu() {
  if (nrPinuWal > -1) {
    if (nrPinuWal < 15) nrPinuWal++;
    else if (nrPinuWal == 15) nrPinuWal = 1;
    Serial.println(nrPinuWal);
  }
}

// Sprawdzenie zmiany stanu czujnika zbliżeniowego
bool zmianaStanu() {
  int stanTeraz = digitalRead(pinCzujnika);
  if (stanTeraz != poprzedniStan) {
    poprzedniStan = stanTeraz;
    if (stanTeraz == 0) {
      liczbaSygnalow++;
      zmienPinWalu();
      sterowaniePrzekaznikiem(5, 9, pinPrzekaznika1);
      return true;
    }
  }
  return false;
}

// Obsługa przycisku włącz/wyłącz
bool aktywny = false;
bool ostatniStanPrzycisku;

void sprawdzPrzycisk() {
  int stanTeraz = digitalRead(pinPrzycisku);
  if (stanTeraz != ostatniStanPrzycisku) {
    if (stanTeraz == LOW) {
      wyswietlacz.fillRect(11, 0, 24, 20, BLACK);
      wyswietlacz.setTextSize(2);
      wyswietlacz.setTextColor(SSD1306_WHITE);
      wyswietlacz.setCursor(11, 0);
      wyswietlacz.println("N");
      wyswietlacz.display();

      aktywny = true;
      liczbaSygnalow = 0;
      nrPinuWal = -1;
      sredniCzas = 0;
      poprzedniStan = digitalRead(pinCzujnika);
    } else {
      aktywny = false;
      wyswietlacz.fillRect(10, 0, 12, 20, BLACK);
      wyswietlacz.setTextSize(2);
      wyswietlacz.setTextColor(SSD1306_WHITE);
      wyswietlacz.setCursor(11, 0);
      wyswietlacz.println("FF");
      wyswietlacz.display();
      digitalWrite(pinPrzekaznika1, HIGH);
    }
    ostatniStanPrzycisku = stanTeraz;
  }
}

// ================== OBSŁUGA TEMPERATURY ==================

// Pomiar temperatury z termistora
float zmierzTemperature() {
  int wartoscAnalogowa = analogRead(PIN_TERMISTORA);
  float napiecie = wartoscAnalogowa * 5.0 / 1023.0;

  float opornosc = REZYSTOR_SZEREGOWY * (1023.0 / wartoscAnalogowa - 1.0);
  float steinhart;
  steinhart = opornosc / ODPORNOSC_NOMINALNA;
  steinhart = log(steinhart);
  steinhart /= WSPOLCZYNNIK_B;
  steinhart += 1.0 / (TEMPERATURA_NOMINALNA + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;
  return steinhart;
}

// ================== FUNKCJE ZAKOMENTOWANE (Ze względu na niewystarczające zasoby systemu w aktualnej konfiguracji sprzętowej. Po odkomentowaniu pojawiają się błędy wykrywania pinów silnika.)==================

/*
// Funkcja do wyświetlania prędkości na ekranie OLED
void wyswietlPredkosc(unsigned long v){
  wyswietlacz.fillRect(50, 40, 60, 15, BLACK);
  wyswietlacz.setTextSize(2);
  wyswietlacz.setTextColor(SSD1306_WHITE);
  wyswietlacz.setCursor(50,40);
  wyswietlacz.println(v);
  wyswietlacz.display();
}
*/

/*
// Pomiar prędkości na podstawie czasu między impulsami
unsigned long czasStartu, czasKonca;
int piny;
bool predkoscObliczona = false;
unsigned long czasOstatniegoWyswietlenia = 0;

void zmierzPredkosc(){
  unsigned long calkowityCzas;

  if(liczbaSygnalow%15 == 0 && !predkoscObliczona){
    czasStartu = millis();
    piny = liczbaSygnalow;
    predkoscObliczona = true;

  } else if(liczbaSygnalow == piny+15 && predkoscObliczona){
    czasKonca = millis();
    calkowityCzas = czasKonca - czasStartu;
    predkoscObliczona = false;
    wyswietlPredkosc(60000/calkowityCzas);
  }
}
*/

// ================== SETUP ==================
void setup() {
  Serial.begin(9600);

  if (!wyswietlacz.begin(SSD1306_SWITCHCAPVCC, ADRES_OLED)) {
    Serial.println(F("Błąd OLED"));
    for (;;);
  }

  pinMode(pinCzujnika, INPUT);
  pinMode(pinPrzekaznika1, OUTPUT);
  pinMode(pinPrzycisku, INPUT_PULLUP);
  digitalWrite(pinPrzekaznika1, HIGH);

  wyswietlacz.clearDisplay();
  wyswietlacz.display();
  wyswietlacz.setTextSize(2);
  wyswietlacz.setTextColor(SSD1306_WHITE);
  wyswietlacz.setCursor(0, 20);
  wyswietlacz.print("T:");
  wyswietlacz.print("24 C");
  wyswietlacz.setCursor(0, 40);
  wyswietlacz.println("O/m:");
  wyswietlacz.setCursor(0, 0);

  ostatniStanPrzycisku = digitalRead(pinPrzycisku);
  if (ostatniStanPrzycisku == LOW) {
    aktywny = true;
    poprzedniStan = digitalRead(pinCzujnika);
    wyswietlacz.println("ON");
  } else {
    wyswietlacz.println("OFF");
  }
  wyswietlacz.display();
}

// ================== PĘTLA GŁÓWNA ==================
unsigned long czasWyswietlaniaTemp = 0;

void loop() {
  // Wyświetlanie temperatury co 1 sekundę
  /*
  if(millis() - czasWyswietlaniaTemp > 1000){
    czasWyswietlaniaTemp = millis();
    wyswietlacz.fillRect(25, 20, 60, 15, BLACK);
    wyswietlacz.setTextSize(2);
    wyswietlacz.setTextColor(SSD1306_WHITE);
    wyswietlacz.setCursor(25, 20);
    wyswietlacz.print(zmierzTemperature());
    wyswietlacz.print(" C");
    wyswietlacz.display();
  }
  */

  sprawdzPrzycisk();

  if (aktywny && nrPinuWal == -1)
    nrPinuWal = ustalPrawdziwyPin();

  if (aktywny && nrPinuWal != -1) {
    zmianaStanu();
    // zmierzPredkosc();
  }
}
