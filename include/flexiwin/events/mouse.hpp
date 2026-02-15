#pragma once

#include <wayland-client.h>

enum flexiwin_pointer_event_type {
  flexiwin_pointer_enter = 1 << 0,
  flexiwin_pointer_leave = 1 << 1,
  flexiwin_pointer_motion = 1 << 2,
  flexiwin_pointer_button = 1 << 4,
  flexiwin_pointer_axis = 1 << 5,
  flexiwin_pointer_axis_source = 1 << 6,
  flexiwin_pointer_axis_stop = 1 << 7,
  flexiwin_pointer_axis_discrete = 1 << 8,
};

struct flexiwin_pointer_event {

  uint32_t event_type;
  uint32_t axis_source;

  float x, y;
  uint32_t button, state;
  uint32_t time, serial;

  struct {
    uint32_t valid;
    wl_fixed_t value;
    int32_t discrete;
  } axis[2];
};
