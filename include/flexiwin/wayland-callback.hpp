#ifndef __FELXON_WAYLAND_CALLBACK__
#define __FELXON_WAYLAND_CALLBACK__
#include <cstdint>
#include <wayland-util.h>

extern const struct wl_seat_listener wl_seat_obj;
extern const struct wl_shm_listener wl_shm_callback;
extern const struct wl_pointer_listener pointer_listener;
extern const struct wl_output_listener display_output_listener;
extern int keyboard_init();
extern void keyboard_destroy();
#endif
