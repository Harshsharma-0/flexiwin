#ifndef __FELXON_WAYLAND_CALLBACK__
#define __FELXON_WAYLAND_CALLBACK__
#include <cstdint>
#include <wayland-util.h>

enum pointer_event_type{
   WL_POINTER_EVENT_ENTER = 1,
   WL_POINTER_EVENT_LEAVE,
   WL_POINTER_EVENT_MOTION,
   WL_POINTER_EVENT_BUTTON,
   WL_POINTER_EVENT_AXIS,
   WL_POINTER_EVENT_AXIS_SOURCE,
   WL_POINTER_EVENT_AXIS_STOP,
   WL_POINTER_EVENT_AXIS_DISCRETE,
};

enum keyboard_event_type{
 WL_KEYBOARD_FOCUS,
 WL_KEYBOARD_LEAVE,
 WL_KEYBOARD_KEY_ON_PRESS,
 WL_KEYBOARD_KEY_ON_RELEASE,

 WL_KEYBOARD_REPEAT_INFO,
 WL_KEYBOARD_MODIFIER
};

enum keyboard_word_type{
  WL_KEY_EVENT_ASCII,
  WL_KEY_EVENT_UNICODE,
  WL_KEY_EVENT_SPECIAL
};

struct pointer_event{

  uint32_t event_type;
  uint32_t axis_source;

  float x , y;
  uint32_t button,state;
  uint32_t time,serial;

  struct{
     uint32_t valid;
     wl_fixed_t value;
     int32_t discrete;
  }axis[2];
};

struct key_event{
  uint32_t event_type;
  uint32_t serial;
  uint32_t key; 
};


extern const struct wl_seat_listener wl_seat_obj;
extern const struct wl_shm_listener wl_shm_callback;
extern const struct wl_pointer_listener pointer_listener;
extern const struct wl_output_listener display_output_listener;
extern int keyboard_init();

#endif

