#include "BleComboMouse.h"


#define LSB(v) ((v >> 8) & 0xff)
#define MSB(v) (v & 0xff)
void BleComboMouse::click(uint8_t b)
{
  _buttons = b;
  move(0,0,0,0);
  _buttons = 0;
  move(0,0,0,0);
}

void BleComboMouse::move(signed char x, signed char y, signed char wheel, signed char hWheel)
{
  if (_keyboard->isConnected())
  {
    uint8_t m[5];
    m[0] = _buttons;
    m[1] = x;
    m[2] = y;
    m[3] = wheel;
    m[4] = hWheel;
    _keyboard->inputMouse->setValue(m, 5);
    _keyboard->inputMouse->notify();
  }
}

void BleComboMouse::send(int state, int16_t x, int16_t y)
{
  if (_keyboard->isConnected())
  {
    uint8_t m[5];
    m[0] = state;
    m[1] = MSB(x);
    m[2] = LSB(x);
    m[3] = MSB(y);
    m[4] = LSB(y);
    _keyboard->inputMouse->setValue(m, 5);
    _keyboard->inputMouse->notify();
  }
}

uint16_t BleComboMouse::scaleToLogical(uint16_t value, uint16_t maxValue, uint16_t logicalMax) const
{
  if (maxValue == 0 || logicalMax == 0) {
    return value;
  }
  if (value > maxValue) {
    value = maxValue;
  }
  return static_cast<uint16_t>((static_cast<uint32_t>(value) * logicalMax) / maxValue);
}

uint16_t BleComboMouse::applyOffsetAndClamp(int32_t value, uint16_t logicalMax) const
{
  if (logicalMax == 0) {
    return static_cast<uint16_t>(value);
  }
  if (value < 0) {
    return 0;
  }
  if (value > logicalMax) {
    return logicalMax;
  }
  return static_cast<uint16_t>(value);
}

void BleComboMouse::setScreenSize(uint16_t width, uint16_t height)
{
  _screenWidth = width;
  _screenHeight = height;
}

void BleComboMouse::setLogicalRange(uint16_t maxX, uint16_t maxY)
{
  _maxX = maxX;
  _maxY = maxY;
}

void BleComboMouse::setCalibrationOffset(int16_t offsetX, int16_t offsetY)
{
  _offsetX = offsetX;
  _offsetY = offsetY;
}

void BleComboMouse::sendAbsolute(uint16_t x, uint16_t y, bool tipSwitch, bool inRange)
{
  uint8_t state = 0;
  if (tipSwitch) {
    state |= 0x01;
  }
  if (inRange) {
    state |= 0x02;
  }
  int32_t adjustedX = static_cast<int32_t>(x) + _offsetX;
  int32_t adjustedY = static_cast<int32_t>(y) + _offsetY;
  uint16_t finalX = applyOffsetAndClamp(adjustedX, _maxX);
  uint16_t finalY = applyOffsetAndClamp(adjustedY, _maxY);
  send(state, static_cast<int16_t>(finalX), static_cast<int16_t>(finalY));
}

void BleComboMouse::sendAbsolutePixel(uint16_t x, uint16_t y, bool tipSwitch, bool inRange)
{
  if (_screenWidth == 0 || _screenHeight == 0) {
    sendAbsolute(x, y, tipSwitch, inRange);
    return;
  }
  uint16_t scaledX = scaleToLogical(x, static_cast<uint16_t>(_screenWidth - 1), _maxX);
  uint16_t scaledY = scaleToLogical(y, static_cast<uint16_t>(_screenHeight - 1), _maxY);
  sendAbsolute(scaledX, scaledY, tipSwitch, inRange);
}

void BleComboMouse::swipeLinear(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint16_t steps, uint16_t delayMs)
{
  if (steps == 0) {
    return;
  }
  for (uint16_t i = 0; i <= steps; ++i) {
    uint16_t x = static_cast<uint16_t>(startX + ((static_cast<int32_t>(endX) - startX) * i) / steps);
    uint16_t y = static_cast<uint16_t>(startY + ((static_cast<int32_t>(endY) - startY) * i) / steps);
    sendAbsolutePixel(x, y);
    delay(delayMs);
  }
  sendAbsolutePixel(endX, endY, false, false);
}

void BleComboMouse::swipeBezier(uint16_t startX, uint16_t startY, uint16_t controlX, uint16_t controlY, uint16_t endX, uint16_t endY, uint16_t steps, uint16_t delayMs)
{
  if (steps == 0) {
    return;
  }
  for (uint16_t i = 0; i <= steps; ++i) {
    float t = static_cast<float>(i) / steps;
    float oneMinusT = 1.0f - t;
    float bx = oneMinusT * oneMinusT * startX + 2.0f * oneMinusT * t * controlX + t * t * endX;
    float by = oneMinusT * oneMinusT * startY + 2.0f * oneMinusT * t * controlY + t * t * endY;
    sendAbsolutePixel(static_cast<uint16_t>(bx), static_cast<uint16_t>(by));
    delay(delayMs);
  }
  sendAbsolutePixel(endX, endY, false, false);
}


void BleComboMouse::buttons(uint8_t b)
{
  if (b != _buttons)
  {
    _buttons = b;
    move(0,0,0,0);
  }
}

void BleComboMouse::press(uint8_t b)
{
  buttons(_buttons | b);
}

void BleComboMouse::release(uint8_t b)
{
  buttons(_buttons & ~b);
}

bool BleComboMouse::isPressed(uint8_t b)
{
  if ((b & _buttons) > 0)
    return true;
  return false;
}
