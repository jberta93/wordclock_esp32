# WordClock ESP32 🕐

🌐 **English** · [Italiano](README.md)

A word clock in **Italian** built on ESP32, WS2812B addressable LEDs and a DS1307 RTC module.
Time is kept by the RTC and automatically synchronized over **NTP** through Wi-Fi.

> Note: the displayed words are in Italian (e.g. *SONO LE SEI E VENTI* — "it's twenty past six"), since the letter panel is laid out in Italian.

## ✨ Features

- **Word-based display in Italian** (e.g. *SONO LE SEI E VENTI*) on a 125-LED matrix.
- **NTP sync over Wi-Fi**: time is fetched from the Internet and stored in the RTC.
- **Wi-Fi setup without recompiling**: on first boot (or if the network is unreachable) an access point named `WordClock_Setup` is created via [WiFiManager](https://github.com/tzapu/WiFiManager); connect to it and enter your credentials from a browser.
- **RTC backup**: once synchronized, the clock keeps running without network thanks to the DS1307.
- **Automatic daily resync** at 04:00.
- **Night mode**: reduced brightness during night hours (23:00 → 07:00).
- **Minute dots**: 4 dedicated LEDs indicate the minutes that are not multiples of 5.
- **Startup animation** that lights LEDs sequentially on power-up.

## 🔌 Required hardware

| Component             | Notes                                             |
|-----------------------|---------------------------------------------------|
| ESP32 (dev board)     | Any Wi-Fi enabled module                          |
| LED strip/matrix      | 125 × WS2812B (NeoPixel), GRB color order         |
| RTC module            | DS1307 (I²C)                                       |
| Power supply          | 5 V, sized for the number of LEDs                 |

### Wiring (default pins)

| Signal         | ESP32 pin |
|----------------|-----------|
| LED data       | GPIO 18   |
| I²C SDA (RTC)  | GPIO 21   |
| I²C SCL (RTC)  | GPIO 22   |

Pins and parameters are changed in the `#define` block at the top of [`hello_word_esp32.ino`](hello_word_esp32.ino).

## ⚙️ Main configuration

All constants are at the beginning of the sketch:

```cpp
#define LED_PIN            18    // LED data pin
#define NUM_LEDS           125   // number of LEDs
#define BRIGHTNESS_DAY     80    // daytime brightness (0-255)
#define BRIGHTNESS_NIGHT    5    // nighttime brightness
#define ORA_INIZIO_NOTTE   23    // night mode start
#define ORA_FINE_NOTTE      7    // night mode end
```

The timezone is set via `TZ_INFO` (`CET-1CEST,M3.5.0,M10.5.0/3` = Europe/Rome with DST).

## 📦 Dependencies

The sketch uses three external libraries (the others — `WiFi`, `Wire`, `time` — ship with the ESP32 core):

| Library         | Author     | Library Manager       |
|-----------------|------------|-----------------------|
| FastLED         | FastLED    | "FastLED"             |
| RTClib          | Adafruit   | "RTClib"              |
| WiFiManager     | tzapu      | "WiFiManager"         |

### Option A — Arduino IDE

1. Install **ESP32** board support (Boards Manager → "esp32" by Espressif Systems).
2. Install the three libraries above from the **Library Manager** (Sketch → Include Library → Manage Libraries).
3. Open `hello_word_esp32.ino`, select your ESP32 board and upload.

### Option B — arduino-cli (reproducible build)

Dependencies are declared in [`sketch.yaml`](sketch.yaml) as a *profile*, so the build is reproducible and version-pinned:

```bash
arduino-cli compile --profile esp32 .
arduino-cli upload  --profile esp32 -p /dev/ttyUSB0 .
```

> The versions in `sketch.yaml` are the tested ones; feel free to update them if needed.

## 🚀 First boot

1. Wire up the hardware and upload the sketch.
2. On power-up the animation runs and the ESP32 looks for a known Wi-Fi network.
3. If none is found, it creates the **`WordClock_Setup`** access point (the first LED turns **red**).
4. Connect to that AP with a phone/PC and enter your network's SSID and password.
5. The ESP32 reconnects, fetches the time over NTP, stores it in the RTC and starts showing the time.

## 🧩 How LED mapping works

The time is rounded to the nearest 5 minutes. The `displayTime()` and `accendi(start, end)`
functions light the LED ranges matching the words (hours, "E"/"MENO", minutes) according to the
physical layout of the letters on the panel. The last 4 LEDs (121–124) are the leftover-minute dots
(1–4 minutes past the multiple of 5).

## 📄 License

Released under the **MIT** license — see the [`LICENSE`](LICENSE) file.
