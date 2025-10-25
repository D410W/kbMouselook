#include <windows.h>
#include <shellapi.h> // For Shell_NotifyIcon
#include "resource.h" // Assuming your icon ID (IDI_MYICON) is here

#define WM_TRAYICON (WM_APP + 1)
#define IDM_EXIT 1001

HWND g_hHiddenWnd = NULL; // Global handle for the hidden window
HINSTANCE g_hInstance = NULL;

// Window Procedure for the hidden window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_TRAYICON:
    if (lParam == WM_RBUTTONUP) {
      // Show context menu on right-click
      HMENU hMenu = CreatePopupMenu();
      AppendMenu(hMenu, MF_STRING, IDM_EXIT, TEXT("Exit"));

      POINT pt;
      GetCursorPos(&pt);

      SetForegroundWindow(hWnd); // Required for menu dismissal
      TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
      PostMessage(hWnd, WM_NULL, 0, 0); // Required for menu dismissal

      DestroyMenu(hMenu);
    }
    break;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDM_EXIT) {
      DestroyWindow(hWnd); // Trigger WM_DESTROY
    }
    break;

  case WM_DESTROY: {
    // Clean up the tray icon
    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1; // Same ID used when adding
    Shell_NotifyIcon(NIM_DELETE, &nid);

    PostQuitMessage(0); // Terminate the message loop
    break;
  }
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// Function to add the icon
void AddTrayIcon(HWND hWnd) {
  NOTIFYICONDATA nid = { 0 };
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.hWnd = hWnd;
  nid.uID = 1; // Unique ID for the icon
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.uCallbackMessage = WM_TRAYICON; // Our custom message
  nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MYICON)); // Load from resource
  lstrcpyn(nid.szTip, TEXT("Numpad Mouse Control"), sizeof(nid.szTip) / sizeof(TCHAR)); // Tooltip

  Shell_NotifyIcon(NIM_ADD, &nid);
}