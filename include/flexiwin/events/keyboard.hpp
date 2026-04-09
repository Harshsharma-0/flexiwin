#pragma once
#include <cstdint>

struct flexiwin_key_event;

enum flexiwin_keyboard_event_type {
  flexiwin_key_focus = 1,
  flexiwin_key_leave,
  flexiwin_key_enter,
  flexiwin_key_pressed,
  flexiwin_key_released,
  flexiwin_key_repeat_info,
  flexiwin_key_modifier
};

enum flexiwin_keyboard_word_type { ascii = 1, unicode, special };
typedef void (*flexiwin_key_callback)(uint32_t event_type, uint32_t time,
                                      uint32_t key, uint32_t serial,
                                      void *data);

struct flexiwin_key_event {
  uint32_t serial;
  void *data;
  flexiwin_key_callback callback;
};
