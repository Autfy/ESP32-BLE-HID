#ifndef ESP32_BLE_COMBO_MOUSE_H
#define ESP32_BLE_COMBO_MOUSE_H
#include "BleComboKeyboard.h"

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_BACK 8
#define MOUSE_FORWARD 16
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE) # For compatibility with the Mouse library

class BleComboMouse {
private:
  BleComboKeyboard* _keyboard;
  uint8_t _buttons;
  uint16_t _screenWidth = 0;
  uint16_t _screenHeight = 0;
  uint16_t _maxX = 10000;
  uint16_t _maxY = 10000;
  int16_t _offsetX = 0;
  int16_t _offsetY = 0;
  void buttons(uint8_t b);
  uint16_t scaleToLogical(uint16_t value, uint16_t maxValue, uint16_t logicalMax) const;
  uint16_t applyOffsetAndClamp(int32_t value, uint16_t logicalMax) const;
public:
  BleComboMouse(BleComboKeyboard* keyboard) { _keyboard = keyboard; };
  void begin(void) {};
  void end(void) {};
  void click(uint8_t b = MOUSE_LEFT);
  void move(signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0);
  void send(int state, int16_t x, int16_t y);
  void setScreenSize(uint16_t width, uint16_t height);
  void setLogicalRange(uint16_t maxX, uint16_t maxY);
  void setCalibrationOffset(int16_t offsetX, int16_t offsetY);
  void sendAbsolute(uint16_t x, uint16_t y, bool tipSwitch = true, bool inRange = true);
  void sendAbsolutePixel(uint16_t x, uint16_t y, bool tipSwitch = true, bool inRange = true);
  void swipeLinear(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint16_t steps, uint16_t delayMs);
  void swipeBezier(uint16_t startX, uint16_t startY, uint16_t controlX, uint16_t controlY, uint16_t endX, uint16_t endY, uint16_t steps, uint16_t delayMs);
  void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
  void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
  bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
};

#endif // ESP32_BLE_COMBO_MOUSE_H
