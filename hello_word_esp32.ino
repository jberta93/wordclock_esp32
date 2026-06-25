#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>
#include "RTClib.h"
#include <Wire.h>
#include <FastLED.h>

// --- CONFIGURAZIONE HARDWARE ---
#define LED_PIN     18      
#define NUM_LEDS    125      
#define LED_TYPE    WS2812B 
#define COLOR_ORDER GRB     

// --- CONFIGURAZIONE LUMINOSITÀ (NIGHT MODE) ---
#define BRIGHTNESS_DAY   80   
#define BRIGHTNESS_NIGHT  5   
#define ORA_INIZIO_NOTTE  23   
#define ORA_FINE_NOTTE     7   

CRGB leds[NUM_LEDS];
RTC_DS1307 rtc;
CRGB color = CRGB(255, 225, 255); 

const char* TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3";

void configModeCallback (WiFiManager *myWiFiManager) {
  // Se entriamo in modalità portale, coloriamo il primo LED di rosso
  leds[0] = CRGB::Red; 
  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); 

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS_DAY);
  FastLED.clear();
  FastLED.show();

  // --- ANIMAZIONE DI AVVIO SEQUENZIALE ---
  // Accende ogni LED uno alla volta
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
    FastLED.show();
    delay(50); // Regola questo valore per cambiare la velocità (10ms * 125 led = 1.25 secondi totali)
  }
  delay(500);
  FastLED.clear(); // Spegne tutto prima di procedere
  FastLED.show();

  if (!rtc.begin()) { 
    Serial.println("RTC non trovato!");
    while (1); 
  }

  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  
  // Se non si connette, apre l'AP "WordClock_Setup"
  if(!wm.autoConnect("WordClock_Setup")) { 
    ESP.restart(); 
  }
  
  // Spegniamo il LED rosso se era acceso
  leds[0] = CRGB::Black;
  FastLED.show();

  // Sincronizzazione Ora
  configTime(0, 0, "pool.ntp.org", "europe.pool.ntp.org");
  setenv("TZ", TZ_INFO, 1);
  tzset();

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, 
                        timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  }
}

void loop() {
  DateTime adesso = rtc.now();
  int oraAttuale = adesso.hour();

  // Gestione luminosità Night Mode
  if (oraAttuale >= ORA_INIZIO_NOTTE || oraAttuale < ORA_FINE_NOTTE) {
    FastLED.setBrightness(BRIGHTNESS_NIGHT);
  } else {
    FastLED.setBrightness(BRIGHTNESS_DAY);
  }

  // Risincronizzazione giornaliera
  if (oraAttuale == 4 && adesso.minute() == 0 && adesso.second() < 5) {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, 
                          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    }
  }

  displayTime(adesso);
  delay(5000); 
}

void displayTime(DateTime t) {
  FastLED.clear(); 

  int h = t.hour();
  int m = t.minute();
  int roundedM = (m / 5) * 5;

  // LED puntini minuti
  int remainder = m % 5;
  if (remainder >= 1) leds[124] = color;
  if (remainder >= 2) leds[123] = color;
  if (remainder >= 3) leds[122] = color;
  if (remainder >= 4) leds[121] = color;

  if (roundedM > 30) h = h + 1;
  if (h > 23) h = 0;
  int displayH = (h > 12) ? h - 12 : h;
  if (displayH == 0) displayH = 12;

  if (roundedM > 0 && roundedM <= 30) {
    accendi(73, 73); 
  } else if (roundedM >= 35) {
    accendi(83, 86); 
  }

  if (displayH == 1) {
    accendi(15, 18); accendi(20, 20); 
  } else {
    accendi(0, 3); accendi(5, 6); 
    switch (displayH) {
      case 2:  accendi(8, 10); break;
      case 3:  accendi(11, 13); break;
      case 4:  accendi(22, 28); break;
      case 5:  accendi(66, 71); break;
      case 6:  accendi(30, 32); break;
      case 7:  accendi(55, 59); break;
      case 8:  accendi(33, 36); break;
      case 9:  accendi(44, 47); break;
      case 10: accendi(61, 65); break;
      case 11: accendi(38, 43); break;
      case 12: accendi(49, 54); break;
    }
  }

  switch (roundedM) {
    case 5:   accendi(99, 104); break;
    case 10:  accendi(113, 117); break;
    case 15:  accendi(90, 91); accendi(93, 98); break;
    case 20:  accendi(105, 109); break;
    case 25:  accendi(99, 109); break;
    case 30:  accendi(77, 81); break;
    case 35:  accendi(99, 109); break;
    case 40:  accendi(105, 109); break;
    case 45:  accendi(90, 91); accendi(93, 98); break;
    case 50:  accendi(113, 117); break;
    case 55:  accendi(99, 104); break;
  }

  FastLED.show(); 
}

void accendi(int start, int end) {
  for (int i = start; i <= end; i++) {
    leds[i] = color;
  }
}