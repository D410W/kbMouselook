#pragma once

namespace ml {

  struct HotkeyAction {
    int vkCode;
    void (*actionFunc0)();
    void (*actionFunc1)(bool);
    void (*actionFunc2)(bool, bool);
    bool param1;

    HotkeyAction(int vk, void (*f)());
    HotkeyAction(int vk, void (*f)(bool), bool p1);
    HotkeyAction(int vk, void (*f)(bool, bool), bool p1);

    void callAction(bool param2) const;
  };

} // namespace ml