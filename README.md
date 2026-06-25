# WordClock ESP32 🕐

Orologio a parole (*word clock*) in **italiano** basato su ESP32, LED indirizzabili WS2812B e modulo RTC DS1307.
L'ora viene mantenuta dall'RTC e sincronizzata automaticamente via **NTP** attraverso il Wi-Fi.

## ✨ Caratteristiche

- **Visualizzazione a parole in italiano** (es. *SONO LE SEI E VENTI*) su una matrice di 125 LED.
- **Sincronizzazione NTP via Wi-Fi**: l'ora viene presa da Internet e salvata nell'RTC.
- **Configurazione Wi-Fi senza ricompilare**: al primo avvio (o se la rete non è raggiungibile) viene creato un access point `WordClock_Setup` tramite [WiFiManager](https://github.com/tzapu/WiFiManager); ci si collega e si inseriscono le credenziali da browser.
- **RTC di backup**: una volta sincronizzato, l'orologio funziona anche senza rete grazie al DS1307.
- **Risincronizzazione giornaliera automatica** alle 04:00.
- **Night mode**: luminosità ridotta nelle ore notturne (23:00 → 07:00).
- **Puntini dei minuti**: 4 LED dedicati indicano i minuti che non sono multipli di 5.
- **Animazione di avvio** sequenziale all'accensione.

## 🔌 Hardware necessario

| Componente            | Note                                              |
|-----------------------|---------------------------------------------------|
| ESP32 (dev board)     | Qualsiasi modulo con Wi-Fi                         |
| Striscia/matrice LED  | 125 × WS2812B (NeoPixel), ordine colori GRB       |
| Modulo RTC            | DS1307 (I²C)                                       |
| Alimentazione         | 5 V adeguata al numero di LED                      |

### Collegamenti (pin di default)

| Segnale        | Pin ESP32 |
|----------------|-----------|
| Dati LED       | GPIO 18   |
| I²C SDA (RTC)  | GPIO 21   |
| I²C SCL (RTC)  | GPIO 22   |

I pin e i parametri si modificano nelle `#define` in cima a [`hello_word_esp32.ino`](hello_word_esp32.ino).

## ⚙️ Configurazione principale

Tutte le costanti sono all'inizio dello sketch:

```cpp
#define LED_PIN            18    // pin dati LED
#define NUM_LEDS           125   // numero di LED
#define BRIGHTNESS_DAY     80    // luminosità diurna (0-255)
#define BRIGHTNESS_NIGHT    5    // luminosità notturna
#define ORA_INIZIO_NOTTE   23    // inizio night mode
#define ORA_FINE_NOTTE      7    // fine night mode
```

Il fuso orario è impostato con `TZ_INFO` (`CET-1CEST,M3.5.0,M10.5.0/3` = Europa/Roma con ora legale).

## 📦 Dipendenze

Lo sketch usa tre librerie esterne (le altre — `WiFi`, `Wire`, `time` — sono incluse nel core ESP32):

| Libreria        | Autore     | Library Manager       |
|-----------------|------------|-----------------------|
| FastLED         | FastLED    | "FastLED"             |
| RTClib          | Adafruit   | "RTClib"              |
| WiFiManager     | tzapu      | "WiFiManager"         |

### Opzione A — Arduino IDE

1. Installa il supporto **ESP32** (Boards Manager → "esp32" di Espressif Systems).
2. Installa le tre librerie qui sopra dal **Library Manager** (Sketch → Include Library → Manage Libraries).
3. Apri `hello_word_esp32.ino`, seleziona la tua board ESP32 e carica.

### Opzione B — arduino-cli (build riproducibile)

Le dipendenze sono dichiarate nel file [`sketch.yaml`](sketch.yaml) come *profilo*, così la compilazione è riproducibile e versionata:

```bash
arduino-cli compile --profile esp32 .
arduino-cli upload  --profile esp32 -p /dev/ttyUSB0 .
```

> Le versioni indicate in `sketch.yaml` sono quelle testate; aggiornale pure se necessario.

## 🚀 Primo avvio

1. Collega l'hardware e carica lo sketch.
2. All'accensione parte l'animazione e l'ESP32 cerca una rete Wi-Fi nota.
3. Se non la trova, crea l'access point **`WordClock_Setup`** (il primo LED diventa **rosso**).
4. Connettiti a quell'AP con telefono/PC, inserisci SSID e password della tua rete.
5. L'ESP32 si riconnette, scarica l'ora via NTP, la salva nell'RTC e inizia a mostrare l'orario.

## 🧩 Come funziona la mappatura dei LED

L'ora viene arrotondata ai 5 minuti più vicini. Le funzioni `displayTime()` e `accendi(start, end)`
accendono gli intervalli di LED corrispondenti alle parole (ore, "E"/"MENO", minuti) secondo la
disposizione fisica delle lettere sul pannello. Gli ultimi 4 LED (121–124) sono i puntini dei minuti
residui (1–4 minuti oltre il multiplo di 5).

## 📄 Licenza

Distribuito sotto licenza **MIT** — vedi il file [`LICENSE`](LICENSE).
