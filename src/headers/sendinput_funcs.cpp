#include "sendinput_funcs.hpp"

void ml::SendMouseMove(int dx, int dy) {
  INPUT input = { 0 };
  input.type = INPUT_MOUSE;
  input.mi.dx = dx;
  input.mi.dy = dy;
  input.mi.dwFlags = MOUSEEVENTF_MOVE;
  SendInput(1, &input, sizeof(INPUT));
}

void ml::SendMouseClick(bool isLeft, bool isHeld) {
  INPUT input = { 0 };

  if (isHeld) {
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = (isLeft ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN);
  }
  else {
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = (isLeft ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP);
  }
  SendInput(1, &input, sizeof(INPUT));
}

void ml::SendMouseScroll(bool isUp, bool isHeld) {
  if (!isHeld) {
    return; // Currently not handling held scrolls
  }

  INPUT input = { 0 };
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_WHEEL;
  input.mi.mouseData = (isUp ? WHEEL_DELTA : -WHEEL_DELTA); // Standard scroll amount (positive = up/forward)
  SendInput(1, &input, sizeof(INPUT));
}

void ml::SendMiddleClick() {
  INPUT input = { 0 };

  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
  SendInput(1, &input, sizeof(INPUT));

  ZeroMemory(&input, sizeof(INPUT)); // Clear previous flags
  input.type = INPUT_MOUSE;
  input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
  SendInput(1, &input, sizeof(INPUT));
}

void ml::SendKeyPress(WORD vkCode, bool press = false) {
  INPUT ip = { 0 };
  ip.type = INPUT_KEYBOARD;

  ip.ki.wVk = vkCode;
  if (press) {
    ip.ki.dwFlags = 0; // 0 indicates key press
  } else {
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
  }
    SendInput(1, &ip, sizeof(INPUT));
}

void ml::SendWalkerMacro() {
  SendKeyPress(0x57, true);
  SendKeyPress(VK_SPACE, true);
  SendKeyPress(VK_CONTROL, true);
}