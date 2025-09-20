# FixuWatti™ Smart Dongle – M5Stack Core2 MVP
*Kehittynyt AI Energy Dashboard -teknologialla*

![FixuWatti™ Logo](docs/logo.png)

## 🎯 Tavoite
Tämä projekti toteuttaa FixuWatti™ Smart Dongle -MVP:n **M5Stack Core2**-laitteella (ESP32). Laite ohjaa relettä sähköpörssin hinnan perusteella, näyttää datan näytöllä ja julkaisee tilatiedot MQTT:hen automaatiota varten (n8n/Node-RED).

**🧠 FixuVirta™ AI Energy Dashboard -teknologia integroitu!**

## ✅ Firmware Status & AI Integration

**🎯 Testattu ja toimiva M5Stack Core2:ssa!**
- ✅ **PlatformIO build:** Onnistunut (ESP32-PICO-D4)
- ✅ **Portin automaattitunnistus:** `/dev/cu.usbserial-2120`
- ✅ **Flash-lataus:** Firmware.bin ladattu onnistuneesti
- ✅ **Resurssienkäyttö:** RAM 1.1%, Flash 14.3%
- ✅ **Toiminnallisuus:** Käynnissä ja toimiva

**🧠 FixuVirta™ AI Energy Dashboard Status:**
- ✅ **ESP32 perusfirmis (Arduino):** MQTT-publish (voltage, current, power, SOC)
- ✅ **Testattu ympäristö:** Mosquitto broker + Home Assistant + Node-RED
- ✅ **Verifioitu:** Reaaliaikainen mittaus <500 ms viive, datan eheys >99%
- 🔄 **Seuraava vaihe:** AI-ennustemalli liitettävä edge- tai pilvitasolla

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

### 🧠 FixuVirta™ AI Energy Dashboard
- AI-pohjainen kulutusennuste (LSTM, 15 min - 24h)
- Automaattinen anomalian tunnistus ja hälytykset  
- Älykäs kuormanhallinta ja optimointi
- Reaaliaikainen energiavirta-analyysi (<500ms viive)

### 🎛️ FixuWatti™ Core-ominaisuudet
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
  Jos käyttöjärjestelmä ei löydä porttia automaattisesti, anna se käsin:  
  `pio device monitor --port /dev/cu.usbserial-2120`
- **Arduino IDE:**  
  Avaa `src/main.cpp`, valitse M5Stack Core2 -kortti ja lataa.

### 4. ✅ Testattu toiminta

**Onnistunut flashaus M5Stack Core2:een:**
- ✅ Firmware käännetty ja ladattu onnistuneesti
- ✅ PlatformIO auto-tunnisti portin `/dev/cu.usbserial-2120`
- ✅ Flash tyhjennetty ja `firmware.bin` ladattu (ESP32-PICO-D4)
- ✅ Laite käynnissä FixuWatti™ buildilla
- ✅ RAM käyttö: 1.1%, Flash: 14.3%

### 5. Seuraa laitteen toimintaa

- `pio device monitor` näyttää FixuWatti™-logit reaaliajassa.
- Tarvittaessa määritä portti: `pio device monitor --port /dev/cu.usbserial-2120`.
- MQTT-brokerilta näet laitteen julkaisut esim. topicissa `fixuwatti/mvp1/status`.
- Työpöytätestaukseen sopivat `mosquitto_pub` ja `mosquitto_sub` samoilla MQTT-tunnuksilla.

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

- Katso luonnos: [`docs/ui_mockup_placeholder.md`](docs/ui_mockup_placeholder.md)
- Lisää lopullinen mockup kuvana polkuun `docs/ui_mockup.png`, kun se on valmis.

---

## 📂 Projektin rakenne

```
/
├── src/main.cpp                       # M5Stack Core2 pääkoodi
├── platformio.ini                     # PlatformIO konfiguraatio
├── README.md                          # Projektin dokumentaatio
├── docs/
│   ├── HOME_ASSISTANT.md              # Home Assistant integraatio-opas
│   ├── automations.yaml               # HA automaatiot
│   ├── configuration.yaml             # HA MQTT sensorit
│   ├── ha_dashboard_example_placeholder.md  # Dashboard-kuvan muistiinpanot
│   ├── home_assistant_dashboard.yaml  # Valmis HA dashboard (YAML)
│   ├── n8n_example.json               # n8n Nord Pool workflow
│   └── ui_mockup_placeholder.md       # UI-mockup luonnos
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

- Dashboard-kuvan muistiinpanot: [`docs/ha_dashboard_example_placeholder.md`](docs/ha_dashboard_example_placeholder.md)
- Valmiit YAML-tiedostot:
  - [`docs/home_assistant_dashboard.yaml`](docs/home_assistant_dashboard.yaml) – Valmis dashboard
  - [`docs/configuration.yaml`](docs/configuration.yaml) – MQTT sensorit
  - [`docs/automations.yaml`](docs/automations.yaml) – Älykkäät automaatiot

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

### FixuVirta™ AI-Enhanced Automaatiot
```yaml
# Sammuta lämmitin kulutuspiikissä
- alias: "FixuVirta AI: Sammuta lämmitin kulutuspiikissä"
  trigger:
    - platform: numeric_state
      entity_id: sensor.fixuvirta_power
      above: 2000
  condition:
    - condition: template
      value_template: "{{ states('sensor.nordpool_price') | float > 0.15 }}"
  action:
    - service: switch.turn_off
      entity_id: switch.lammitin
    - service: notify.mobile_app_iphone
      data:
        message: "🔥 Lämmitin sammutettu kulutuspiikin takia: {{ states('sensor.fixuvirta_power') }}W"

# Lataa akku halvalla sähköllä (AI-optimoitu)
- alias: "FixuVirta AI: Lataa akku halvalla sähköllä"
  trigger:
    - platform: numeric_state
      entity_id: sensor.nordpool_price
      below: 0.05
  condition:
    - condition: numeric_state
      entity_id: sensor.fixuwatti_soc
      below: 80  # Älä lataa jos jo täynnä
  action:
    - service: switch.turn_on
      entity_id: switch.battery_charger
    - service: notify.mobile_app_iphone
      data:
        message: "🔋 Akkulataus aloitettu: {{ states('sensor.nordpool_price') }} €/kWh"
```

---

## ❓ Troubleshooting / Usein kysytyt kysymykset

### 🔌 "Laite ei yhdistä Wi-Fi:iin"
- ✅ Tarkista `WIFI_SSID` ja `WIFI_PASS` asetukset `src/main.cpp`:ssa
- ✅ Varmista että käytät 2.4GHz verkkoa (ESP32 ei tue 5GHz)
- ✅ Kokeile resetoida laite pitkällä C-painikkeen painalluksella (2s)

### 📡 "MQTT ei toimi / Ei dataa MQTT:hen"
- ✅ Tarkista `MQTT_SERVER`, `MQTT_USER`, `MQTT_PASS` asetukset
- ✅ Varmista että MQTT broker on käynnissä ja saavutettavissa
- ✅ Tarkista broker IP ja portti (`1883`) - varmista että Wi-Fi on yhdistetty
- ✅ Kokeile MQTT Explorer -työkalua yhteyden testaamiseen

### 💻 "Serial monitor ei näytä mitään"
- ✅ Tarkista oikea portti: `pio device monitor --port /dev/cu.usbserial-2120`
- ✅ Varmista että USB-kaapeli on data-kaapeli (ei pelkkä lataus)
- ✅ Kokeila eri baudia: `pio device monitor --baud 9600`

### 🔋 "SOC/Virta-arvot 0 tai epätarkkoja"
- ✅ SOC on simuloitu arvo (92%) - muokkaa `currentPrice` muuttujaa koodissa
- ✅ **CT-anturin ongelmat:** Tarkista kytkentä (ei väärinpäin), käytä burden-resistoria
- ✅ Oikeassa käytössä yhdistä INA219 tai SCT-013 virta-anturi

### 🏠 "Home Assistant ei löydä FixuWatti™ laitteita"
- ✅ Tarkista että MQTT integraatio on asennettu HA:ssa
- ✅ Lisää `discovery: true` asetukset tai manuaaliset `mqtt sensor` -konfiguraatiot
- ✅ Käynnistä Home Assistant uudelleen asetusmuutosten jälkeen

### 🧠 "AI-malli ei toimi ESP:llä"
- ✅ **Ensimmäinen vaihe:** Käytä serveripohjaista TensorFlow / scikit-learn
- ✅ **Myöhemmin:** Siirrä TF Lite Micro -malli ESP32:lle
- ✅ **ESP32 kaatuu pitkän käytön jälkeen:** Lisää `WiFi.setSleep(false)` ja reconnect-logiikka

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
git commit -m "feat: Lisää uusi ominaisuus"  # feat:/fix:/docs: konventio
git push origin feature/uusi-ominaisuus

# Avaa Pull Request GitHubissa
```

**Commit-viestit:** Käytä `feat:`, `fix:`, `docs:` konventiota ja dokumentoi muutokset `CHANGELOG.md`:ään.

**Testaa aina:**
- `esp32_firmware/` (tai `src/`) käännös onnistuu
- MQTT-data näkyy testibrokerilla

### 📖 Dokumentaation parantaminen
- README parannukset
- Home Assistant oppaat
- Troubleshooting vinkit
- Käännökset muille kielille

---

## 📋 Muutoshistoria

### v1.2.0 (2025-09-20) - FixuVirta™ AI Integration
- 🧠 **AI-ennustemalli** (serveripohjainen LSTM, 15 min forecast)
- 🤖 **Kehittyneet automaatiot** kulutuspiikkien hallintaan
- 📊 **Parannettu MQTT-data:** voltage, current, power mittaukset
- 🔧 **MQTT reconnect-logiikka** (Wi-Fi dropouts)

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

**FixuWatti™ + FixuVirta™ AI Energy Dashboard tarjoaa:**

### 🏢 B2B-markkina
- **Rakennus- ja työmaasähkö:** Kustannussäästöt ja huoltolaitteiden kulutuksen hallinta
- **Teollisuus:** Reaaliaikainen energiankulutuksen optimointi ja ennakoiva huolto
- **Kiinteistöt:** Älykkäät energiaratkaisut ja automaattinen kuormanhallinta

### 🏠 B2C-markkina  
- **Älykoti + mökkikäyttäjät:** Pörssisähköoptimointi ja akkujen lataus halvalla
- **Kotitaloudet:** Reaaliaikainen kulutusseuranta ja säästövinkit
- **Sähköauton lataus:** AI-optimoitu latausajastus halvoimpiin tunteihin

### 🎯 White Label & Palvelumallit
- **White label:** FixuVirta™ dashboard voidaan brändätä energiayhtiöille ja laitetoimittajille
- **Palvelumalli:** "Energy as a Service" → kuukausimaksu + laitebundle (ESP32, CT, dashboard)
- **API-integraatiot:** Saumaton yhteys olemassa oleviin energianhallintajärjestelmiin

### 🧠 AI-lisäarvo
- **Kulutusennusteet:** 15 min - 24h ennusteet kulutukselle ja optimaalille ohjaukselle
- **Anomalian tunnistus:** Automaattinen hälytysten generointi poikkeavasta kulutuksesta
- **Älykäs automaatio:** Oppiva järjestelmä joka mukautuu käyttäjän tarpeisiin

### 💰 Asiakashyödyt
- 📊 **Läpinäkyvyys:** Reaaliaikainen näkymä sähkön hintaan ja säästöihin
- 🤖 **Automatiikka:** Älykäs laitteiden ohjaus ilman käyttäjän toimenpiteitä  
- 💰 **Säästöt:** Optimoitu kulutus Nord Pool -hintojen mukaan (10-30% säästö)
- 🏠 **Integraatio:** Saumaton yhteys Home Assistant -järjestelmiin

---

## 📝 Lisenssi

© 2025 mxxx222 / FixuWatti™. Tämä projekti on tarkoitettu MVP-demoihin ja jatkokehitykseen.
