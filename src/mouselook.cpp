#define WINVER 0x0500 // Required for SendInput
#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <chrono>

#include <windows.h>
#include <mmsystem.h> // For multimedia timers

#include "headers/hotkey_action.hpp"
#include "headers/sendinput_funcs.hpp"

// --- Configuration ---
const int MOUSE_SPEED = 10; // Pixels to move per interval
const UINT TIMER_INTERVAL_MS = 20; // Milliseconds between movements (lower = smoother)
const UINT_PTR MOVEMENT_TIMER_ID = 1; // ID for our movement timer
const float FAST_MULTIPLIER = 3.0f; // Speed when Shift is held
const float SLOW_MULTIPLIER = 0.1f; // Speed when Ctrl is held

// --- Global Variables ---
HHOOK g_keyboardHook = NULL;
std::map<int, bool> g_keyState; // Map to track pressed keys (VK_CODE -> isDown)
int g_deltaX = 0; // Current horizontal movement per tick
int g_deltaY = 0; // Current vertical movement per tick
UINT g_timerID = 0; // CHANGED: Use UINT for timeSetEvent ID

// ===========================================
// ============== Hotkey Setup ===============
// ===========================================

std::array<ml::HotkeyAction, 6> actionMap = {
  ml::HotkeyAction{ VK_NUMPAD0, ml::SendMouseClick, true },  // Numpad 0 -> Left Click
  ml::HotkeyAction{ VK_ADD,     ml::SendMouseClick, false }, // Numpad + -> Right Click
  ml::HotkeyAction{ VK_NUMPAD7, ml::SendMouseScroll, true }, // Numpad 7 -> Scroll Up
  ml::HotkeyAction{ VK_NUMPAD9, ml::SendMouseScroll, false }, // Numpad 9 -> Scroll Down
  ml::HotkeyAction{ VK_MULTIPLY, ml::SendMiddleClick }, // Numpad * -> Scroll Middle Click
  ml::HotkeyAction{ VK_NUMPAD1, ml::SendWalkerMacro } // Numpad 1 -> Walker Macro
};

// ===========================================
// ========= Mouse Movement Handling =========
// ===========================================

VOID CALLBACK HighResTimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
  // --- Add Timing Measurement ---
  // static auto lastCallTime = std::chrono::high_resolution_clock::now();
  // auto now = std::chrono::high_resolution_clock::now();
  // auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCallTime).count();
  // lastCallTime = now;
    
  // Print occasionally to avoid spamming the console too much
  // static int callCount = 0;
  // if (++callCount % 20 == 0) { // Print every 100 calls
  //     std::cout << "  -> TimerProc Interval: ~" << elapsedMs << " ms (ID: " << uTimerID << ")" << std::endl << std::flush;
  // }
  // --- End Timing Measurement ---


  // Safety check
  if (uTimerID != g_timerID || g_timerID == 0) {
      return;
  }
  if (g_deltaX == 0 && g_deltaY == 0) {
    if (g_timerID != 0) {
      timeKillEvent(g_timerID);
      g_timerID = 0;
      // std::cout << "  -> TimerProc detected zero delta, stopping Timer (ID: " << uTimerID << ")" << std::endl << std::flush;
    }
    return;
  }

  float currentMultiplier = 1.0f;
  bool slowHeld = (GetAsyncKeyState(VK_LMENU) & 0x8000) || (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
  bool fastHeld = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_RCONTROL) & 0x8000);
  if (fastHeld) { currentMultiplier = FAST_MULTIPLIER; }
  else if (slowHeld) { currentMultiplier = SLOW_MULTIPLIER; }

  int currentDeltaX = static_cast<int>(g_deltaX * currentMultiplier);
  int currentDeltaY = static_cast<int>(g_deltaY * currentMultiplier);
  if (currentDeltaX == 0 && g_deltaX != 0) currentDeltaX = (g_deltaX > 0) ? 1 : -1;
  if (currentDeltaY == 0 && g_deltaY != 0) currentDeltaY = (g_deltaY > 0) ? 1 : -1;
  if (currentDeltaX != 0 || currentDeltaY != 0) {
    ml::SendMouseMove(currentDeltaX, currentDeltaY);
  }
}

// UpdateMovementDeltas
void UpdateMovementDeltas() {
  g_deltaX = 0;
  g_deltaY = 0;
  // [Numpad key checks unchanged]
  if (g_keyState[VK_NUMPAD8]) g_deltaY = -MOUSE_SPEED;
  if (g_keyState[VK_NUMPAD5]) g_deltaY = MOUSE_SPEED;
  if (g_keyState[VK_NUMPAD4]) g_deltaX = -MOUSE_SPEED;
  if (g_keyState[VK_NUMPAD6]) g_deltaX = MOUSE_SPEED;

  bool anyNumpadKeyDown = false;
  std::vector<int> numpadKeys = { VK_NUMPAD8, VK_NUMPAD5, VK_NUMPAD4, VK_NUMPAD6 };
  for (int key : numpadKeys) {
    if (g_keyState.count(key) && g_keyState[key]) {
      anyNumpadKeyDown = true;
      break;
    }
  }

  // --- Use timeSetEvent / timeKillEvent ---
  if (anyNumpadKeyDown && g_timerID == 0) {
    // A numpad key is pressed, and timer isn't running -> Start it
    UINT newTimerID = timeSetEvent(TIMER_INTERVAL_MS, 0, HighResTimerProc, 0, TIME_PERIODIC);
    if (newTimerID == 0) {
      std::cerr << "!!! Failed to set high-resolution timer!" << std::endl << std::flush;
    }
    else {
      g_timerID = newTimerID; // Store the new ID
      // std::cout << ">>> High-res timer started (Assigned ID: " << g_timerID << ")" << std::endl << std::flush; // MODIFIED LOG
    }
  }
  else if (!anyNumpadKeyDown && g_timerID != 0) {
    // No numpad keys are pressed, and timer is running -> Stop it
    MMRESULT killResult = timeKillEvent(g_timerID); // Store result
    // std::cout << "<<< Attempting to stop high-res timer (ID: " << g_timerID << "). Result: " << (killResult == TIMERR_NOERROR ? "OK" : "Error") << std::endl << std::flush; // MODIFIED LOG
    g_timerID = 0; // Reset the ID *regardless* of kill result
    g_deltaX = 0;
    g_deltaY = 0;
  }
}

// Low-Level Keyboard Hook Procedure
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*)lParam;
    int vkCode = pkbhs->vkCode;
    bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    bool handled = false;

    // Check for Escape key to exit
    if (vkCode == VK_DELETE && isKeyDown) {
      std::cout << "Escape pressed. Exiting..." << std::endl;
      PostQuitMessage(0); // Signal the message loop to terminate
      return 1; // Block Escape key
    }

    // --- Check Action Map (NEW SECTION) ---
    for (const auto& hk : actionMap) {
	    const auto& key = hk.vkCode;
      if (vkCode == key) {
		    hk.callAction(isKeyDown);
        handled = true;
        break;
      }
    }
    // --- End Action Map Check ---

    // Check if it's one of our Numpad keys
    std::vector<int> numpadKeys = {
      VK_NUMPAD8, VK_NUMPAD5, VK_NUMPAD4, VK_NUMPAD6
    };

    for (int key : numpadKeys) {
      if (vkCode == key) {
        // Update the state of the key
        g_keyState[vkCode] = isKeyDown;
        UpdateMovementDeltas(); // Recalculate movement needed
        handled = true;
        break; // Stop checking other keys
      }
    }

    if (handled) {
      // Only block if it was a numpad key or escape
      return 1;
    }
  }

  // Call the next hook in the chain
  return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

int main() {
    FreeConsole();

    // Request a higher timer resolution (e.g., 1ms)
    if (timeBeginPeriod(1) != TIMERR_NOERROR) {
      std::cerr << "Warning: Could not set high timer resolution." << std::endl;
    }

    std::cout << "Starting Numpad Mouse..." << std::endl;
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    // ... (error check) ...

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    // --- Cleanup ---
    if (g_keyboardHook != NULL) {
      UnhookWindowsHookEx(g_keyboardHook);
    }
    if (g_timerID != 0) { // Kill timer if it was still running
      timeKillEvent(g_timerID);
    }
    timeEndPeriod(1); // Release the timer resolution request
    // --- End Cleanup ---

    std::cout << "Numpad Mouse stopped." << std::endl;
    return 0;
}