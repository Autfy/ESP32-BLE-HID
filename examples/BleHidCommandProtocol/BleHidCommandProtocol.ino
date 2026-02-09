#include <BleCombo.h>

BleComboKeyboard keyboard("esp32-ble-hid", "Espressif", 100);
BleComboMouse mouse(&keyboard);

class CommandCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string value = pCharacteristic->getValue();
    if (value.empty()) {
      return;
    }

    String cmd = String(value.c_str());
    cmd.trim();
    if (cmd.length() == 0) {
      return;
    }

    if (cmd.startsWith("TEXT ")) {
      String payload = cmd.substring(5);
      keyboard.print(payload);
      return;
    }

    if (cmd.startsWith("SET_SCREEN")) {
      int w = 0;
      int h = 0;
      if (sscanf(cmd.c_str(), "SET_SCREEN %d %d", &w, &h) == 2) {
        mouse.setScreenSize(static_cast<uint16_t>(w), static_cast<uint16_t>(h));
      }
      return;
    }

    if (cmd.startsWith("SET_LOGICAL")) {
      int maxX = 0;
      int maxY = 0;
      if (sscanf(cmd.c_str(), "SET_LOGICAL %d %d", &maxX, &maxY) == 2) {
        mouse.setLogicalRange(static_cast<uint16_t>(maxX), static_cast<uint16_t>(maxY));
      }
      return;
    }

    if (cmd.startsWith("OFFSET")) {
      int dx = 0;
      int dy = 0;
      if (sscanf(cmd.c_str(), "OFFSET %d %d", &dx, &dy) == 2) {
        mouse.setCalibrationOffset(static_cast<int16_t>(dx), static_cast<int16_t>(dy));
      }
      return;
    }

    if (cmd.startsWith("TAP")) {
      int x = 0;
      int y = 0;
      if (sscanf(cmd.c_str(), "TAP %d %d", &x, &y) == 2) {
        mouse.sendAbsolutePixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y));
        delay(30);
        mouse.sendAbsolutePixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), false, false);
      }
      return;
    }

    if (cmd.startsWith("LONG")) {
      int x = 0;
      int y = 0;
      int duration = 0;
      if (sscanf(cmd.c_str(), "LONG %d %d %d", &x, &y, &duration) == 3) {
        mouse.sendAbsolutePixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y));
        delay(duration);
        mouse.sendAbsolutePixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), false, false);
      }
      return;
    }

    if (cmd.startsWith("SWIPE")) {
      int x0 = 0;
      int y0 = 0;
      int x1 = 0;
      int y1 = 0;
      int steps = 0;
      int delayMs = 0;
      if (sscanf(cmd.c_str(), "SWIPE %d %d %d %d %d %d", &x0, &y0, &x1, &y1, &steps, &delayMs) == 6) {
        mouse.swipeLinear(static_cast<uint16_t>(x0), static_cast<uint16_t>(y0), static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(steps), static_cast<uint16_t>(delayMs));
      }
      return;
    }

    if (cmd.startsWith("BEZIER")) {
      int x0 = 0;
      int y0 = 0;
      int cx = 0;
      int cy = 0;
      int x1 = 0;
      int y1 = 0;
      int steps = 0;
      int delayMs = 0;
      if (sscanf(cmd.c_str(), "BEZIER %d %d %d %d %d %d %d %d", &x0, &y0, &cx, &cy, &x1, &y1, &steps, &delayMs) == 8) {
        mouse.swipeBezier(static_cast<uint16_t>(x0), static_cast<uint16_t>(y0), static_cast<uint16_t>(cx), static_cast<uint16_t>(cy), static_cast<uint16_t>(x1), static_cast<uint16_t>(y1), static_cast<uint16_t>(steps), static_cast<uint16_t>(delayMs));
      }
      return;
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE HID command protocol example.");

  keyboard.setMTU(185);
  keyboard.setPreferredConnectionParams(0x06, 0x12);

  static CommandCallbacks callbacks;
  keyboard.setCallbacks(&callbacks);
  keyboard.begin();
  mouse.begin();
}

void loop() {
  delay(1000);
}
