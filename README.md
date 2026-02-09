# ESP32 BLE HID 键盘/鼠标与双向通信库
本库让 ESP32 开发板作为蓝牙 HID 设备运行，支持键盘输入、绝对坐标触控（鼠标/触摸屏）、媒体按键与双向通信，并可与 Wi-Fi 功能共存。

## 项目说明
本项目为独立维护版本，与原先项目无从属或关联关系，功能与路线由本项目自行规划与实现。

## 硬件兼容
- ESP32（经典双核）
- ESP32-C3
- ESP32-S3

建议使用 Arduino-ESP32 2.x 及以上版本，三种芯片均可正常编译运行。

## 安装
- 在 Arduino IDE 中选择 “Sketch” -> “Include Library” -> “Add .ZIP Library...”，选择下载的库文件。
- 安装后可在 “File” -> “Examples” -> “ESP32 BLE HID” 中查看示例。

## 功能介绍
- BLE HID 键盘输入（普通按键与媒体按键）。
- BLE HID 绝对坐标触控（Android 等设备会将 0~10000 逻辑坐标映射到屏幕）。
- BLE 双向通信特征值（自定义服务/特征值可读写）。
- 支持与 Wi-Fi 并行工作（BLE HID 与联网验证示例）。
- 已完成：自定义指令协议、Write Without Response/MTU/连接参数优化、滑动轨迹插值算法、坐标偏移校正。

## 接口说明（核心）
### BleComboKeyboard
- `BleComboKeyboard(deviceName, manufacturer, batteryLevel)`：创建 BLE HID 设备。
- `begin()` / `end()`：启动/停止 BLE。
- `isConnected()`：判断是否已连接。
- `setBatteryLevel(level)`：设置电量。
- `setMTU(mtu)`：设置 BLE MTU（建议 185 左右）。
- `setPreferredConnectionParams(minPreferred, maxPreferred)`：设置连接参数偏好（影响延迟与稳定性）。

### BleComboMouse（触控/鼠标）
- `click(button)` / `press(button)` / `release(button)`：鼠标按键操作。
- `move(x, y, wheel, hWheel)`：相对移动（传统鼠标）。
- `send(state, x, y)`：发送原始绝对坐标（0~10000），`state` 为触控状态位。
- `setScreenSize(width, height)`：设置屏幕像素分辨率，用于自动换算。
- `setLogicalRange(maxX, maxY)`：设置逻辑坐标范围，默认 10000x10000。
- `setCalibrationOffset(offsetX, offsetY)`：坐标校正偏移（用于左侧/顶部校正）。
- `sendAbsolute(x, y, tipSwitch, inRange)`：发送逻辑坐标（不做换算）。
- `sendAbsolutePixel(x, y, tipSwitch, inRange)`：输入像素坐标，自动换算到逻辑坐标后发送。
- `swipeLinear(startX, startY, endX, endY, steps, delayMs)`：匀速滑动插值。
- `swipeBezier(startX, startY, controlX, controlY, endX, endY, steps, delayMs)`：贝塞尔曲线滑动插值。

> 说明：绝对坐标模式下，Android 会将 0~10000 逻辑坐标映射到屏幕；若要“自动分辨率转换”，请先调用 `setScreenSize()`，再使用 `sendAbsolutePixel()`。
> 坐标偏移校正可以用于解决左边/顶部点击偏差问题，使用 `setCalibrationOffset()` 进行微调。

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

## 绝对坐标/自动分辨率转换示例
```cpp
BleComboMouse mouse(&keyboard);
mouse.setScreenSize(1080, 2400);     // 设置屏幕像素
mouse.setLogicalRange(10000, 10000); // 可选，默认即为 10000
mouse.setCalibrationOffset(0, 0);    // 坐标偏移校正（可选）

// 发送像素坐标（自动换算到 0~10000）
mouse.sendAbsolutePixel(540, 1200);

// 发送逻辑坐标（不做换算）
mouse.sendAbsolute(5000, 5000);
```

## App -> ESP32 自定义指令协议（推荐）
通过 BLE 自定义特征值发送 ASCII 指令，每条指令以换行分隔（或单次写入一条指令）。推荐使用 `Write Without Response` 以减少延迟。

### 指令格式
```
SET_SCREEN <width> <height>
SET_LOGICAL <maxX> <maxY>
OFFSET <dx> <dy>
TAP <x> <y>
LONG <x> <y> <durationMs>
SWIPE <x0> <y0> <x1> <y1> <steps> <delayMs>
BEZIER <x0> <y0> <cx> <cy> <x1> <y1> <steps> <delayMs>
TEXT <utf8_text>
```

### 示例
```
SET_SCREEN 1080 2400
OFFSET 5 0
TAP 540 1200
SWIPE 200 1800 900 400 30 8
BEZIER 200 1800 500 1200 900 400 40 6
TEXT Hello ESP32
```

完整示例见：`examples/BleHidCommandProtocol/BleHidCommandProtocol.ino`。

## 注意事项
- 本库仅供学习使用，请勿用于非法用途，否则后果自负。
- 用户明确理解并同意，任何违反法律法规、侵犯他人合法权益的行为，均与本项目及其开发者无关，后果由用户自行承担。
