# kbMouselook

A simple converter of low-level signals written in c++.
Converts (only) the numpad keys into other input signals.

Currently only has Windows support.

## Binds

### Send mouse actions

Numpad 4, 5, 6, 8: **Mouse movement (as in 'left', 'down', 'right', 'up').**\
[Modifiers]: 'CONTROL' speeds up, and 'ALT' slows down the cursor.

Numpad 0, Numpad '+': **Left and Right clicks.**

Numpad 7, 9: **Scroll up, Scroll down.**

Numpad '*': **Middle button click.**

### Send key presses

Numpad '1': **(macro) Sends the keyboard keys 'W', 'SPACE', 'CONTROL', and leaves them pressed.**

## Compilation

This project is compiled with CMake.
