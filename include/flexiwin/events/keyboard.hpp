#pragma once
#include <cstdint>

enum flexiwin_keyboard_event_type {
  flexiwin_key_focus = 1,
  flexiwin_key_leave,
  flexiwin_key_on_press,
  flexiwin_key_on_release,
  flexiwin_key_repeat_info,
  flexiwin_key_modifier
};

enum flexiwin_keyboard_word_type { ascii = 1, unicode, special };

struct flexiwin_key_event {
  uint32_t event_type;
  uint32_t serial;
  uint32_t key;
};
