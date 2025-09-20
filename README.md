# FixuWatti™ Smart Dongle – M5Stack Core2 MVP

![FixuWatti™ Logo](docs/logo.png)

## 🎯 Tavoite
Tämä projekti toteuttaa FixuWatti™ Smart Dongle -MVP:n **M5Stack Core2**-laitteella (ESP32). Laite ohjaa relettä sähköpörssin hinnan perusteella, näyttää datan näytöllä ja julkaisee tilatiedot MQTT:hen automaatiota varten (n8n/Node-RED).

## ✅ Firmware Status

**🎯 Testattu ja toimiva M5Stack Core2:ssa!**
- ✅ **PlatformIO build:** Onnistunut (ESP32-PICO-D4)
- ✅ **Portin automaattitunnistus:** `/dev/cu.usbserial-2120`
- ✅ **Flash-lataus:** Firmware.bin ladattu onnistuneesti
- ✅ **Resurssienkäyttö:** RAM 1.1%, Flash 14.3%
- ✅ **Toiminnallisuus:** Käynnissä ja toimiva

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
- MQTT-integraatio Home Assistantiin
- OTA-päivitykset
- Nord Pool sähköpörssin integraatio
- Suomenkielinen energiadashboard

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
  ```bash
  pio run -t upload    # Käännä ja lataa
  pio device monitor   # Avaa serial monitor
  ```
- **Arduino IDE:**  
  Avaa `src/main.cpp`, valitse M5Stack Core2 -kortti ja lataa.

### 4. ✅ Testattu toiminta

**Onnistunut flashaus M5Stack Core2:een:**
- ✅ Firmware käännetty ja ladattu onnistuneesti
- ✅ PlatformIO auto-tunnisti portin `/dev/cu.usbserial-2120`
- ✅ Flash tyhjennetty ja `firmware.bin` ladattu (ESP32-PICO-D4)
- ✅ Laite käynnissä FixuWatti™ buildilla
- ✅ RAM käyttö: 1.1%, Flash: 14.3%

### 4. Seuraa laitteen toimintaa serial monitorilla

Jos haluat seurata M5Stack Core2:n toimintaa reaaliajassa:

```bash
pio device monitor
```

Tai jos laite on jo liitetty tiettyyn porttiin:

```bash
pio device monitor --port /dev/cu.usbserial-2120
```

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

**Huom:** UI-mockup (`docs/ui_mockup.png`) on placeholder. Lisää oikea mockup-kuva tähän tiedostoon myöhemmin.

---

## 📂 Projektin rakenne

```
/
├── src/main.cpp              # M5Stack Core2 pääkoodi
├── platformio.ini            # PlatformIO konfiguraatio
├── README.md                 # Projektin dokumentaatio
├── docs/
│   ├── HOME_ASSISTANT.md     # Home Assistant integraatio-opas
│   ├── home_assistant_dashboard.yaml  # Valmis HA dashboard
│   ├── configuration.yaml    # HA MQTT sensorit
│   ├── automations.yaml      # HA automaatiot
│   ├── n8n_example.json      # n8n Nord Pool workflow
│   └── ui_mockup.png         # UI-mockup kuva
```

---

## 🏠 Home Assistant Integraatio

Täydellinen energiadashboard suomalaiselle markkinalle:
- 📊 **Nord Pool** sähköpörssin hinnat
- 🔋 **FixuWatti™** akun SOC ja releen tila
- 🤖 **Automaatiot** hinta-hälytyksille
- 🇫🇮 **Suomenkielinen** käyttöliittymä

**Katso:** [`docs/HOME_ASSISTANT.md`](docs/HOME_ASSISTANT.md) - Täydellinen asennusopas

### 🖥️ Dashboard Esimerkki

![Home Assistant Dashboard](docs/ha_dashboard_example.png)

*Valmis energiadashboard näyttää reaaliajassa Nord Pool hinnat, FixuWatti™ SOC, releen tilan ja automaatiot.*

**Valmiit tiedostot:**
- [`docs/home_assistant_dashboard.yaml`](docs/home_assistant_dashboard.yaml) - Valmis dashboard
- [`docs/configuration.yaml`](docs/configuration.yaml) - MQTT sensorit  
- [`docs/automations.yaml`](docs/automations.yaml) - Älykkäät automaatiot

---

## 🤖 Automaatio Esimerkkejä

### Tipping Point Automatiikka
```yaml
# Rele ON kun hinta < 9 snt/kWh
- alias: "FixuWatti™ Auto ON"
  trigger:
    - platform: numeric_state
      entity_id: sensor.nordpool_kwh_fi_eur_3_10_0_current_price
      below: 0.09
  condition:
    - condition: state
      entity_id: sensor.fixuwatti_status
      state: "AUTO"
  action:
    - service: switch.turn_on
      entity_id: switch.fixuwatti_rele

# Rele OFF kun hinta > 25 snt/kWh
- alias: "FixuWatti™ Auto OFF"
  trigger:
    - platform: numeric_state
      entity_id: sensor.nordpool_kwh_fi_eur_3_10_0_current_price
      above: 0.25
  condition:
    - condition: state
      entity_id: sensor.fixuwatti_status
      state: "AUTO"
  action:
    - service: switch.turn_off
      entity_id: switch.fixuwatti_rele
```

### Hinta-hälytykset
```yaml
# Ilmoitus kun sähkö halpaa
- alias: "Halpa sähkö ilmoitus"
  trigger:
    - platform: numeric_state
      entity_id: sensor.nordpool_kwh_fi_eur_3_10_0_current_price
      below: 0.05  # 5 snt/kWh
  action:
    - service: notify.mobile_app_iphone
      data:
        message: "💰 Sähkö halpaa! {{ states('sensor.nordpool_kwh_fi_eur_3_10_0_current_price') }} €/kWh"
        title: "FixuWatti™ Säästömahdollisuus"
```

---

## ❓ Troubleshooting / Usein kysytyt kysymykset

### 🔌 "Laite ei yhdistä Wi-Fi:iin"
- ✅ Tarkista `WIFI_SSID` ja `WIFI_PASS` asetukset `src/main.cpp`:ssa
- ✅ Varmista että käytät 2.4GHz verkkoa (ESP32 ei tue 5GHz)
- ✅ Kokeile resetoida laite pitkällä C-painikkeen painalluksella (2s)

### 📡 "MQTT ei toimi"
- ✅ Tarkista `MQTT_SERVER`, `MQTT_USER`, `MQTT_PASS` asetukset
- ✅ Varmista että MQTT broker on käynnissä ja saavutettavissa
- ✅ Kokeile MQTT Explorer -työkalua yhteyden testaamiseen

### 💻 "Serial monitor ei näytä mitään"
- ✅ Tarkista oikea portti: `pio device monitor --port /dev/cu.usbserial-2120`
- ✅ Varmista että USB-kaapeli on data-kaapeli (ei pelkkä lataus)
- ✅ Kokeila eri baudia: `pio device monitor --baud 9600`

### 🔋 "SOC näyttää väärää arvoa"
- ✅ SOC on simuloitu arvo (92%) - muokkaa `currentPrice` muuttujaa koodissa
- ✅ Oikeassa käytössä yhdistä INA219 tai muu akku-anturi

### 🏠 "Home Assistant ei löydä FixuWatti™ laitteita"
- ✅ Tarkista että MQTT integraatio on asennettu HA:ssa
- ✅ Lisää `configuration.yaml` sensorit manuaalisesti
- ✅ Käynnistä Home Assistant uudelleen asetusmuutosten jälkeen

---

## 🤝 Osallistu kehitykseen

FixuWatti™ on avoin projekti! Tervetuloa mukaan:

### 🐛 Raportoi bugeja
- Avaa [GitHub Issue](https://github.com/pyyhkija93/fixuwatti-smart-dongle/issues)
- Kerro tarkat tiedot: laitteisto, versio, virheilmoitus

### 💡 Ehdota parannuksia
- Feature requestit GitHub Issueissa
- Keskustele [Discussions](https://github.com/pyyhkija93/fixuwatti-smart-dongle/discussions) -osiossa

### 🔧 Koodin kontribuutiot
```bash
# Fork projekti GitHubissa
git clone https://github.com/SINUN-KÄYTTÄJÄ/fixuwatti-smart-dongle.git
cd fixuwatti-smart-dongle

# Tee muutokset
git checkout -b feature/uusi-ominaisuus
# ... muokkaa koodia ...
git commit -m "Lisää uusi ominaisuus"
git push origin feature/uusi-ominaisuus

# Avaa Pull Request GitHubissa
```

### 📖 Dokumentaation parantaminen
- README parannukset
- Home Assistant oppaat
- Troubleshooting vinkit
- Käännökset muille kielille

---

## 📋 Muutoshistoria

### v1.1.0 (2025-09-20)
- ✅ **Home Assistant integraatio** täydellä dokumentaatiolla
- ✅ **Valmis dashboard** ja automaatiot Nord Pool:lle
- ✅ **PlatformIO rakenne** korjattu (`src/main.cpp`)
- ✅ **Firmware testattu** M5Stack Core2:ssa
- ✅ **MQTT-sensorit** ja template-laskutoimitukset

### v1.0.0 (2025-09-20)
- 🎯 **Ensimmäinen MVP julkaisu**
- ⚡ **M5Stack Core2** tuki
- 📡 **MQTT integraatio** PubSubClient:lla
- 🎮 **Kosketuspainikkeet** A/B/C toiminnallisuudella
- 🔄 **Tipping point logiikka** sähkön hinnalle
- 🌐 **n8n workflow** Nord Pool integraatioon

---

## 💡 Liiketoimintapotentiaali

FixuWatti™ tarjoaa asiakkaille:
- 📊 **Läpinäkyvyys:** Reaaliaikainen näkymä sähkön hintaan ja säästöihin
- 🤖 **Automatiikka:** Älykäs laitteiden ohjaus ilman käyttäjän toimenpiteitä  
- 💰 **Säästöt:** Optimoitu kulutus Nord Pool -hintojen mukaan
- 🏠 **Integraatio:** Saumaton yhteys Home Assistant -järjestelmiin

---

## 📝 Lisenssi

© 2025 mxxx222 / FixuWatti™. Tämä projekti on tarkoitettu MVP-demoihin ja jatkokehitykseen.
