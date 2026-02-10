#include <BleCombo.h>
#include <WiFi.h>

// 修改为你的 Wi-Fi 信息
const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";

BleComboKeyboard keyboard("esp32-ble-hid", "Espressif", 100);
BleComboMouse mouse(&keyboard);

WiFiClient client;

bool wifiValidated = false;
bool hidReported = false;
unsigned long lastWiFiAttempt = 0;
const unsigned long wifiRetryIntervalMs = 10000;

class MyCallbacks : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) override {}
  void onWrite(BLECharacteristic *pCharacteristic) override {}
};

bool validateInternet() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  if (!client.connect("example.com", 80)) {
    return false;
  }
  client.print("GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n");
  unsigned long start = millis();
  while (!client.available() && millis() - start < 3000) {
    delay(10);
  }
  bool ok = client.available();
  client.stop();
  return ok;
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  Serial.println("[WiFi] Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(200);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WiFi] Connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[WiFi] Connect failed, retry later.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE HID + WiFi validation example.");

  MyCallbacks myCallbacks;
  keyboard.setCallbacks(&myCallbacks);
  keyboard.begin();
  mouse.begin();
}

void loop() {
  if (!wifiValidated && millis() - lastWiFiAttempt > wifiRetryIntervalMs) {
    lastWiFiAttempt = millis();
    connectWiFi();
    wifiValidated = validateInternet();
    Serial.println(wifiValidated ? "[WiFi] Validation OK." : "[WiFi] Validation pending.");
  }

  if (wifiValidated && Keyboard.isConnected() && !hidReported) {
    keyboard.print("BLE HID OK");
    mouse.click();
    hidReported = true;
    Serial.println("[BLE] HID report sent.");
  }

  delay(1000);
}
