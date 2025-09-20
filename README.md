# FixuWatti™ Smart Dongle – M5Stack Core2 MVP

![FixuWatti™ Logo](docs/logo.png)

## 🎯 Tavoite
Tämä projekti toteuttaa FixuWatti™ Smart Dongle -MVP:n **M5Stack Core2**-laitteella (ESP32). Laite ohjaa relettä sähköpörssin hinnan perusteella, näyttää datan näytöllä ja julkaisee tilatiedot MQTT:hen automaatiota varten (n8n/Node-RED).

---

## 🛠 Tekninen rakenne

- **Laitteisto:**  
  - M5Stack Core2 (ESP32)
  - Rele (simuloitu LED:llä, GPIO25)
  - Kosketuspainikkeet (A/B/C, M5Touch)
  - Näyttö (Core2 LCD)
  - *(Valinnainen)* INA219 -virta-anturi

- **Ohjelmointi:**  
  - Arduino C++ (PlatformIO/Arduino IDE)
  - MQTT: PubSubClient
  - OTA-päivitykset: ArduinoOTA

---

## ⚡ Ominaisuudet

- Tipping point -logiikka sähkön hinnan ja raja-arvojen mukaan
- Releen ohjaus: Auto/Manual-tila
- Kolme kosketuspainiketta: Quick Kill, Auto/Manual, Status/Reset
- Reaaliaikainen näyttö: sähkön hinta, SOC, säästö, Wi-Fi/MQTT-status
- MQTT-integraatio
- OTA-päivitykset

---

## 🏗 Asennusohjeet

### 1. Tarvittavat kirjastot (PlatformIO)

```ini
[env:m5stack-core2]
platform = espressif32
board = m5stack-core2
framework = arduino
lib_deps =
  m5stack/M5Core2
  knolleary/PubSubClient
  bblanchon/ArduinoJson
  thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays
  johnnyb/INA219 (jos virta-anturi käytössä)
```

### 2. Wi-Fi ja MQTT-tiedot

Muokkaa `main.cpp`-tiedoston alkuun Wi-Fi ja MQTT-asetukset.

### 3. Käännä & Lataa

- **PlatformIO:**  
  `pio run -t upload`
- **Arduino IDE:**  
  Avaa `main.cpp`, valitse M5Stack Core2 -kortti ja lataa.

---

## 📡 MQTT-topic-rakenne

| Topic                               | Sisältö           |
|------------------------------------- |-------------------|
| `fixuwatti/mvp1/status`              | JSON: yhteys, tila, tila-ikoni |
| `fixuwatti/mvp1/sensor/price`        | Sähkönhinta €/kWh |
| `fixuwatti/mvp1/sensor/soc`          | Akkutaso %        |
| `fixuwatti/mvp1/switch/rele1/state`  | ON/OFF            |
| `fixuwatti/mvp1/switch/rele1/command`| Ohjausviesti      |

---

## 🖲 Painikkeiden logiikka

- **A (vasen):** Rele ON/OFF toggle (ohittaa automatiikan).
- **B (keskellä):** Vaihda Auto/Manual; ikoni vaihtuu.
- **C (oikea):** Näytä sähkön hinta/SOC; pitkä painallus = reset.

---

## 🔄 Tipping Point -logiikka

```c
if (mode == AUTO) {
    if (price < LOW_CENTS) rele = ON;
    else if (price > HIGH_CENTS) rele = OFF;
}
```
*(LOW_CENTS ja HIGH_CENTS määritellään koodissa)*

---

## 🌐 n8n/Node-RED-esimerkki

- n8n hakee Nord Pool -sähkön hinnan HTTP nodella.
- Julkaisee hinnan MQTT:hen (`fixuwatti/mvp1/sensor/price`).
- Laskee tipping pointin ja ohjaa relettä MQTT:llä (`fixuwatti/mvp1/switch/rele1/command`).

Katso esimerkkiflow tiedostosta [`docs/n8n_example.json`](docs/n8n_example.json).

---

## 🖼 UI-kuvakaappaus (mockup)

![UI-mockup](docs/ui_mockup.png)

---

## 📂 Projektin rakenne

```
/
├── main.cpp
├── platformio.ini
├── README.md
├── docs/
│   ├── n8n_example.json
│   └── ui_mockup.png
```

---

## 📝 Lisenssi

© 2025 mxxx222 / FixuWatti™. Tämä projekti on tarkoitettu MVP-demoihin ja jatkokehitykseen.
