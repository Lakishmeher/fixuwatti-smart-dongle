#include <M5Core2.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#ifdef USE_INA219
#include <Adafruit_INA219.h>
#endif

// ---- KONFIGUROITAVAT ARVOT ----
#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASS     "YOUR_WIFI_PASSWORD"
#define MQTT_SERVER   "192.168.1.100"
#define MQTT_PORT     1883
#define MQTT_USER     "mqttuser"
#define MQTT_PASS     "mqttpass"

#define LOW_CENTS     0.09   // €/kWh, tipping point ON
#define HIGH_CENTS    0.25   // €/kWh, tipping point OFF

#define RELAY_PIN     25     // Simuloidaan LED:llä

// MQTT topicit
#define TOPIC_STATUS        "fixuwatti/mvp1/status"
#define TOPIC_PRICE         "fixuwatti/mvp1/sensor/price"
#define TOPIC_SOC           "fixuwatti/mvp1/sensor/soc"
#define TOPIC_RELAY_STATE   "fixuwatti/mvp1/switch/rele1/state"
#define TOPIC_RELAY_CMD     "fixuwatti/mvp1/switch/rele1/command"

// ---- MUUTTUJAT ----
enum Mode { AUTO, MANUAL };
Mode mode = AUTO;

float currentPrice = 0.15;
float soc = 92.0;
bool relayState = false;
bool mqttConnected = false;
bool wifiConnected = false;
uint32_t lastMqtt = 0;
uint32_t lastScreen = 0;
uint32_t lastSocQuery = 0;

#ifdef USE_INA219
Adafruit_INA219 ina219;
#endif

WiFiClient espClient;
PubSubClient mqtt(espClient);

// ---- LOGIIKKA ----
void setRelay(bool on) {
    relayState = on;
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
    mqtt.publish(TOPIC_RELAY_STATE, on ? "ON" : "OFF", true);
}

void tippingLogic() {
    if (mode == AUTO) {
        if (currentPrice < LOW_CENTS && !relayState) setRelay(true);
        else if (currentPrice > HIGH_CENTS && relayState) setRelay(false);
    }
}

// ---- MQTT CALLBACK ----
void mqttCallback(char* topic, byte* payload, unsigned int len) {
    String msg;
    for (uint16_t i = 0; i < len; i++) msg += (char)payload[i];
    if (String(topic) == TOPIC_RELAY_CMD) {
        if (msg == "ON") { setRelay(true); }
        else if (msg == "OFF") { setRelay(false); }
    }
    if (String(topic) == TOPIC_PRICE) {
        currentPrice = msg.toFloat();
    }
    if (String(topic) == TOPIC_SOC) {
        soc = msg.toFloat();
    }
}

// ---- WIFI/MQTT ----
void setupWifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < 20) {
        delay(500);
        tries++;
    }
    wifiConnected = WiFi.status() == WL_CONNECTED;
}

void reconnectMqtt() {
    while (!mqtt.connected()) {
        if (mqtt.connect("FixuWattiMVP", MQTT_USER, MQTT_PASS)) {
            mqtt.subscribe(TOPIC_RELAY_CMD);
            mqtt.subscribe(TOPIC_PRICE);
            mqtt.subscribe(TOPIC_SOC);
            mqttConnected = true;
        } else {
            delay(2000);
        }
    }
}

// ---- UI ----
void drawUI() {
    M5.Lcd.fillScreen(BLACK);
    // Yläosa: logo + status
    M5.Lcd.setTextColor(WHITE); M5.Lcd.setTextSize(2);
    M5.Lcd.drawString("FixuWatti", 10, 5);
    if (wifiConnected) {
        M5.Lcd.fillCircle(200, 20, 7, mqttConnected?GREEN:YELLOW);
    } else {
        M5.Lcd.fillCircle(200, 20, 7, RED);
    }
    // Keskiosa: rele, hinta, SOC
    M5.Lcd.setTextColor(WHITE); M5.Lcd.setTextSize(2);
    M5.Lcd.drawString(String("Rele: ") + (relayState?"ON":"OFF"), 20, 50);
    M5.Lcd.drawString("Hinta: " + String(currentPrice,2) + " €/kWh", 20, 90);
    M5.Lcd.drawString("SOC: " + String(soc,1) + " %", 20, 130);
    // Tilaikoni
    if (mode == AUTO)
      M5.Lcd.drawString("AUTO", 210, 50);
    else
      M5.Lcd.drawString("MAN", 210, 50);
    // Alaosa: painikkeet
    M5.Lcd.fillRoundRect(10, 200, 70, 35, 8, 0x39C4); // A
    M5.Lcd.fillRoundRect(90, 200, 70, 35, 8, 0x8C71); // B
    M5.Lcd.fillRoundRect(170, 200, 70, 35, 8, 0xFBE4); // C
    M5.Lcd.setTextColor(BLACK); M5.Lcd.setTextSize(2);
    M5.Lcd.drawString("A", 42, 210);
    M5.Lcd.drawString("B", 122, 210);
    M5.Lcd.drawString("C", 202, 210);
}

// ---- BUTTONS ----
void handleTouch() {
    M5.update();
    if (M5.BtnA.wasPressed()) { // Quick Kill
        mode = MANUAL;
        setRelay(!relayState);
    }
    if (M5.BtnB.wasPressed()) { // Auto/Man
        mode = (mode == AUTO) ? MANUAL : AUTO;
        tippingLogic();
    }
    if (M5.BtnC.wasPressed()) { // Status/Reset short
        drawUI();
    }
    if (M5.BtnC.pressedFor(2000)) { // Long press
        ESP.restart();
    }
}

// ---- MQTT STATUS ----
void publishStatus() {
    StaticJsonDocument<128> doc;
    doc["wifi"] = wifiConnected;
    doc["mqtt"] = mqttConnected;
    doc["relay"] = relayState;
    doc["mode"] = (mode==AUTO) ? "AUTO" : "MANUAL";
    char buf[128];
    serializeJson(doc, buf);
    mqtt.publish(TOPIC_STATUS, buf, true);
}

#ifdef USE_INA219
void publishINA219() {
    float busvoltage = ina219.getBusVoltage_V();
    float current_mA = ina219.getCurrent_mA();
    StaticJsonDocument<128> doc;
    doc["busvoltage"] = busvoltage;
    doc["current"] = current_mA;
    char buf[128];
    serializeJson(doc, buf);
    mqtt.publish("fixuwatti/mvp1/sensor/ina219", buf);
}
#endif

// ---- SETUP & LOOP ----
void setup() {
    M5.begin();
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
#ifdef USE_INA219
    ina219.begin();
#endif
    setupWifi();
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    mqtt.setCallback(mqttCallback);
    drawUI();
}

void loop() {
    if (!wifiConnected || WiFi.status() != WL_CONNECTED) {
        setupWifi();
    }
    if (!mqtt.connected()) {
        reconnectMqtt();
    }
    mqtt.loop();

    // Päivitä UI
    if (millis() - lastScreen > 2000) {
        drawUI();
        lastScreen = millis();
    }
    // Tipping point
    tippingLogic();

    // MQTT status
    if (millis() - lastMqtt > 10000) {
        publishStatus();
#ifdef USE_INA219
        publishINA219();
#endif
        lastMqtt = millis();
    }
    // Painikkeet
    handleTouch();
}
