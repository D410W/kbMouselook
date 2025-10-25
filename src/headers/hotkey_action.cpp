#include "hotkey_action.hpp"

ml::HotkeyAction::HotkeyAction(int vk, void (*f)())
  : vkCode(vk), actionFunc0(f), actionFunc1(nullptr), actionFunc2(nullptr), param1(false) {
}
ml::HotkeyAction::HotkeyAction(int vk, void (*f)(bool), bool p1)
  : vkCode(vk), actionFunc0(nullptr), actionFunc1(f), actionFunc2(nullptr), param1(p1) {
}
ml::HotkeyAction::HotkeyAction(int vk, void (*f)(bool, bool), bool p1)
  : vkCode(vk), actionFunc0(nullptr), actionFunc1(nullptr), actionFunc2(f), param1(p1) {
}

void ml::HotkeyAction::callAction(bool param2 = true) const {
  if (actionFunc0 != nullptr) {
    actionFunc0();
  }
  else if (actionFunc1 != nullptr) {
    actionFunc1(param1);
  }
  else if (actionFunc2 != nullptr) {
    actionFunc2(param1, param2);
  }
}