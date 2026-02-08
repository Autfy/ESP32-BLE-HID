# ESP32 BLE HID 键盘/鼠标与双向通信库
本库让 ESP32 开发板作为蓝牙 HID 设备运行，支持键盘输入、鼠标点击与双向通信，并可与 Wi-Fi 功能共存。

## 硬件兼容
- ESP32（经典双核）
- ESP32-C3
- ESP32-S3

建议使用 Arduino-ESP32 2.x 及以上版本，三种芯片均可正常编译运行。

## 安装
- 在 Arduino IDE 中选择 “Sketch” -> “Include Library” -> “Add .ZIP Library...”，选择下载的库文件。
- 安装后可在 “File” -> “Examples” -> “ESP32 BLE HID” 中查看示例。

## 快速示例（BLE 键盘 + 鼠标）
```cpp
#include <BleCombo.h>
BleComboKeyboard keyboard("cheese-ble-hid", "Espressif", 100);
BleComboMouse mouse(&keyboard);
class MyCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic* pCharacteristic) { }
  void onWrite(BLECharacteristic *pCharacteristic) { }
};
void setup() {
  Serial.begin(115200);
  Serial.println("Starting work!");
  MyCallbacks myCallbacks;
  keyboard.setCallbacks(&myCallbacks);
  keyboard.begin();
  mouse.begin(); 
}
void loop() {
  if(Keyboard.isConnected()) {
   mouse.click();
  }
  delay(2000);
}
```

## BLE HID + Wi-Fi 连接与验证示例
该示例在 BLE HID 正常工作时，同时连接 Wi-Fi 并通过访问 `example.com` 进行联网验证。请先替换 SSID 与密码。
```cpp
#include <BleCombo.h>
#include <WiFi.h>

const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";

BleComboKeyboard keyboard("esp32-ble-hid", "Espressif", 100);
BleComboMouse mouse(&keyboard);
```
完整代码见：`examples/BleHidWifiValidation/BleHidWifiValidation.ino`。

## 注意事项
- 本库仅供学习使用，请勿用于非法用途，否则后果自负。
- 用户明确理解并同意，任何违反法律法规、侵犯他人合法权益的行为，均与本项目及其开发者无关，后果由用户自行承担。

## 联系方式
- QQ 群：710985269

## 致谢
本项目基于以下项目的工作实现：
- [ESP32-BLE-Mouse](https://github.com/T-vK/ESP32-BLE-Mouse)
- [ESP32-BLE-Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard)
- [ESP32-BLE-Abs-Mouse](https://github.com/sobrinho/ESP32-BLE-Abs-Mouse)
- [ESP32-BLE-Combo](https://github.com/blackketter/ESP32-BLE-Combo)
