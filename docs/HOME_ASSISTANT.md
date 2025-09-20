# Home Assistant Energiadashboard - FixuWatti™ integraatio

![Home Assistant + FixuWatti](docs/ha_dashboard_mockup.png)

## 🏠 Yleiskatsaus

Tämä opas näyttää kuinka rakentaa Home Assistant -energiadashboard suomalaiselle markkinalle integroimalla:
- **Nord Pool** sähköpörssin hinnat
- **FixuWatti™** MQTT-data (ESP32 dongle)
- **Oma kulutus** ja akun SOC
- **Suomenkielinen käyttöliittymä**

---

## 📋 Vaiheittainen asennus

### 1. 🏗 Home Assistant -asennus

**Asennusvaihtoehdot:**
```bash
# Docker (suositeltu)
docker run -d --name homeassistant --restart=unless-stopped \
  -e TZ=Europe/Helsinki -v /PATH_TO_CONFIG:/config \
  --network=host ghcr.io/home-assistant/home-assistant:stable

# Raspberry Pi OS
curl -fsSL https://get.home-assistant.io | sh
```

**Pääsy:** `http://homeassistant.local:8123` tai `http://[IP]:8123`

### 2. 🏪 HACS-asennus (Community Store)

1. Lataa HACS: https://hacs.xyz/docs/setup/download
2. Kopioi `/config/custom_components/hacs/`
3. Käynnistä Home Assistant uudelleen
4. Lisää integraatio: Asetukset > Laitteet ja palvelut > HACS

### 3. ⚡ Nord Pool -integraatio

**HACS:in kautta:**
1. HACS > Integraatiot > Etsi "Nord Pool"
2. Asenna "Nord Pool" by @custom-components
3. Käynnistä uudelleen
4. Asetukset > Laitteet ja palvelut > Lisää integraatio > "Nord Pool"
5. Valitse alue: **FI** (Suomi)
6. Valuutta: **EUR**

**Tuloksena sensorit:**
- `sensor.nordpool_kwh_fi_eur_3_10_0` (tuntihinnat)
- `sensor.nordpool_kwh_fi_eur_3_10_0_current_price` (nykyinen hinta)

### 4. 📡 FixuWatti™ MQTT-integraatio

**Lisää MQTT broker:**
1. Asetukset > Laitteet ja palvelut > Lisää integraatio > "MQTT"
2. Broker IP: `192.168.1.100` (tai oma MQTT server)
3. Käyttäjä/salasana FixuWatti™ asetuksista

**Lisää configuration.yaml:**
```yaml
mqtt:
  sensor:
    # FixuWatti™ akun SOC
    - name: "FixuWatti SOC"
      state_topic: "fixuwatti/mvp1/sensor/soc"
      unit_of_measurement: "%"
      device_class: "battery"
      icon: "mdi:battery"
      
    # FixuWatti™ MQTT-sähkön hinta
    - name: "FixuWatti Sähkön Hinta"
      state_topic: "fixuwatti/mvp1/sensor/price"
      unit_of_measurement: "€/kWh"
      device_class: "monetary"
      icon: "mdi:lightning-bolt"
      
    # FixuWatti™ tila JSON
    - name: "FixuWatti Status"
      state_topic: "fixuwatti/mvp1/status"
      value_template: "{{ value_json.mode }}"
      icon: "mdi:chip"

  switch:
    # FixuWatti™ releen ohjaus
    - name: "FixuWatti Rele"
      state_topic: "fixuwatti/mvp1/switch/rele1/state"
      command_topic: "fixuwatti/mvp1/switch/rele1/command"
      payload_on: "ON"
      payload_off: "OFF"
      icon: "mdi:power-socket-eu"
```

**Käynnistä uudelleen** tai käytä "Tarkista konfiguraatio" ja "Lataa uudelleen".

### 5. 📊 Energiadashboard luominen

**Uusi dashboard:**
1. Sivupalkki > "Yleiskatsaus" 
2. Oikea yläkulma > "Muokkaa dashboardia"
3. Lisää kortti > "By Card Type"

#### A) Entities Card (Yhteenveto)
```yaml
type: entities
title: "⚡ FixuWatti™ Energia"
show_header_toggle: false
entities:
  - entity: sensor.nordpool_kwh_fi_eur_3_10_0_current_price
    name: "Nord Pool hinta nyt"
    icon: mdi:currency-eur
  - entity: sensor.fixuwatti_sahkon_hinta
    name: "MQTT hinta"
    icon: mdi:flash
  - entity: sensor.fixuwatti_soc
    name: "Akun lataus"
    icon: mdi:battery-charging
  - entity: switch.fixuwatti_rele
    name: "Rele (Auto/Manual)"
    icon: mdi:power-socket-eu
  - entity: sensor.fixuwatti_status
    name: "Tilа"
    icon: mdi:information
```

#### B) History Graph (Hintakehitys)
```yaml
type: history-graph
title: "📈 Sähkön hinnan kehitys"
entities:
  - entity: sensor.nordpool_kwh_fi_eur_3_10_0_current_price
    name: "Nord Pool (€/kWh)"
  - entity: sensor.fixuwatti_sahkon_hinta  
    name: "FixuWatti MQTT"
hours_to_show: 24
refresh_interval: 60
```

#### C) Gauge Card (SOC-mittari)
```yaml
type: gauge
title: "🔋 Akun SOC"
entity: sensor.fixuwatti_soc
min: 0
max: 100
needle: true
segments:
  - from: 0
    color: red
  - from: 20
    color: yellow  
  - from: 50
    color: green
```

#### D) Statistics Card (Tilastot)
```yaml
type: statistics-graph
title: "📊 Energia tilastot (24h)"
entities:
  - entity: sensor.nordpool_kwh_fi_eur_3_10_0_current_price
    name: "Hinta €/kWh"
  - entity: sensor.fixuwatti_soc
    name: "SOC %"
chart_type: line
period: hour
days_to_show: 1
```

### 6. 🇫🇮 Suomenkielinen käyttöliittymä

**Kieli ja alue:**
1. Profiili (vasemmassa alakulmassa) > "Asetukset"
2. Kieli: **Suomi**
3. Aikavyöhyke: **Europe/Helsinki**
4. Valuutta: **EUR (€)**
5. Yksikkö: **Metric**

**Päivämäärä:** `dd.mm.yyyy` (suomalainen)

### 7. 🎨 Moderni UI (valinnainen)

**Mushroom Cards (HACS):**
```bash
# HACS > Frontend > Etsi "Mushroom"
# Asenna "Mushroom Cards"
```

**Mushroom Energy Card -esimerkki:**
```yaml
type: custom:mushroom-entity-card
entity: sensor.nordpool_kwh_fi_eur_3_10_0_current_price
name: "Sähkön hinta"
icon: mdi:lightning-bolt
icon_color: amber
primary_info: name
secondary_info: state
```

---

## 🔧 Automaatiot (valinnainen)

### Hinta-alarm
```yaml
automation:
  - alias: "FixuWatti™ Korkea sähkön hinta"
    trigger:
      - platform: numeric_state
        entity_id: sensor.nordpool_kwh_fi_eur_3_10_0_current_price
        above: 0.25  # €/kWh
    action:
      - service: notify.mobile_app_[your_phone]
        data:
          message: "⚡ Sähkön hinta korkea: {{ states('sensor.nordpool_kwh_fi_eur_3_10_0_current_price') }} €/kWh"
          title: "FixuWatti™ Hinta-alarm"
```

### Akun matala SOC
```yaml
automation:
  - alias: "FixuWatti™ Matala akku"
    trigger:
      - platform: numeric_state
        entity_id: sensor.fixuwatti_soc
        below: 20  # %
    action:
      - service: notify.mobile_app_[your_phone]
        data:
          message: "🔋 FixuWatti™ akku matala: {{ states('sensor.fixuwatti_soc') }}%"
          title: "Akku-alarm"
```

---

## 📱 Mobiilisovellus

**Home Assistant Companion:**
- iOS: App Store "Home Assistant"
- Android: Play Store "Home Assistant"
- Sama dashboard toimii mobiilissa
- Push-notifikaatiot automaatioille

---

## 🔗 Hyödylliset linkit

- **Nord Pool integraatio:** https://github.com/custom-components/nordpool
- **HACS:** https://hacs.xyz/
- **Mushroom Cards:** https://github.com/piitaya/lovelace-mushroom
- **Home Assistant dokumentaatio:** https://www.home-assistant.io/docs/

---

## 📊 Valmis dashboard-malli

Katso `docs/` kansiosta valmiit YAML-tiedostot:
- `home_assistant_dashboard.yaml` - Koko dashboard
- `automations.yaml` - Automaatiot
- `configuration.yaml` - MQTT sensorit

**Asennus:** Kopioi sisältö Home Assistant konfiguraatioosi ja käynnistä uudelleen.

---

© 2025 FixuWatti™ - Suomalainen älyenergia-ratkaisu 🇫🇮
