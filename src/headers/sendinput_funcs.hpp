#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <chrono>

#include <windows.h>

namespace ml {
  void SendMouseMove(int dx, int dy);

  void SendMouseClick(bool isLeft, bool isHeld);

  void SendMouseScroll(bool isUp, bool isHeld);

  void SendMiddleClick();

  void SendKeyPress(WORD vkCode, bool press);

  void SendWalkerMacro();
};