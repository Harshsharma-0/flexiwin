#include "flexiwin/wayland-callback.hpp"
#include "flexiwin/common.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>

static void pointer_button(void *data, struct wl_pointer *pointer,
                           uint32_t serial, uint32_t time, uint32_t button,
                           uint32_t state) {

  struct flexiwin_pointer_event *_state = (struct flexiwin_pointer_event *)data;

  _state->event_type |= flexiwin_pointer_button;
  _state->serial = serial;
  _state->button = button;
  _state->state = state;
  _state->time = time;
}

static void pointer_enter(void *data, struct wl_pointer *wl_pointer,
                          uint32_t serial, struct wl_surface *surface,
                          wl_fixed_t surface_x, wl_fixed_t surface_y) {
  struct flexiwin_pointer_event *state = (struct flexiwin_pointer_event *)data;

  state->event_type |= flexiwin_pointer_enter;
  state->x = wl_fixed_to_double(surface_x);
  state->y = wl_fixed_to_double(surface_y);
  state->serial = serial;
};

static void pointer_leave(void *data, struct wl_pointer *wl_pointer,
                          uint32_t serial, struct wl_surface *surface) {

  struct flexiwin_pointer_event *state = (struct flexiwin_pointer_event *)data;

  state->event_type |= flexiwin_pointer_leave;
  state->serial = serial;
};

static void pointer_motion(void *data, struct wl_pointer *wl_pointer,
                           uint32_t time, wl_fixed_t surface_x,
                           wl_fixed_t surface_y) {

  struct flexiwin_pointer_event *state = (struct flexiwin_pointer_event *)data;

  state->event_type |= flexiwin_pointer_motion;
  state->x = wl_fixed_to_double(surface_x);
  state->y = wl_fixed_to_double(surface_y);
  state->time = time;
};

static void pointer_axis(void *data, struct wl_pointer *wl_pointer,
                         uint32_t time, uint32_t axis, wl_fixed_t value) {

  struct flexiwin_pointer_event *state = (struct flexiwin_pointer_event *)data;

  state->event_type |= flexiwin_pointer_axis;
  state->time = time;
  state->axis[axis].valid = 1;
  state->axis[axis].value = value;
};

static void pointer_frame(void *data, struct wl_pointer *wl_pointer) {

  /*
    // TODO: propagate the event data to the state manager pointer event queue

    //  xdg_toplevel_move(xdg_toplevel, wl_seat, serial);
    // struct window_state* info = (window_state*)data;
    //  xdg_toplevel_move(info->xdg_surface_toplevel,info->display_seat,serial);
    //
    xdg_toplevel_show_window_menu(info->xdg_surface_toplevel,
    info->display_seat, serial, 0, 0);
    //
    xdg_toplevel_resize(info->xdg_surface_toplevel, info->display_seat,
                        info->serial, 2);
    //
    xdg_toplevel_resize(info->xdg_surface_toplevel, info->display_seat,
                        pointer_state.serial, 3);
    // flexon::memset64(&pointer_state,0,loop);

    //   pointer_state = statemanager::getNextPointerQueue();
    // utility::strings::memset64(pointer_state,0,7);
    */

};

static void pointer_axis_source(void *data, struct wl_pointer *wl_pointer,
                                uint32_t axis_source) {

  struct flexiwin_pointer_event *state = (struct flexiwin_pointer_event *)data;
  state->event_type |= flexiwin_pointer_axis_source;
  state->axis_source = axis_source;
};

static void pointer_axis_stop(void *data, struct wl_pointer *wl_pointer,
                              uint32_t time, uint32_t axis) {

  struct flexiwin_pointer_event *state = (struct flexiwin_pointer_event *)data;
  state->event_type |= flexiwin_pointer_axis_stop;
  state->time = time;
  state->axis[axis].valid = 1;
};

static void pointer_axis_discrete(void *data, struct wl_pointer *wl_pointer,
                                  uint32_t axis, int32_t discrete) {

  struct flexiwin_pointer_event *state = (struct flexiwin_pointer_event *)data;

  state->event_type |= flexiwin_pointer_axis_discrete;
  state->axis[axis].valid = 1;
  state->axis[axis].discrete = discrete;
};

const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_enter,
    .leave = pointer_leave,
    .motion = pointer_motion,
    .button = pointer_button,
    .axis = pointer_axis,
    .frame = pointer_frame,
    .axis_source = pointer_axis_source,
    .axis_stop = pointer_axis_stop,
    .axis_discrete = pointer_axis_discrete,
};

static struct xkb_state *key_xkb_state = nullptr;
static struct xkb_context *key_xkb_context = nullptr;
static struct xkb_keymap *key_xkb_keymap = nullptr;

int keyboard_init() {
  key_xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (key_xkb_context == nullptr)
    return -1;
  return 0;
};

void keyboard_destroy() {
  if (key_xkb_context != nullptr)
    xkb_context_unref(key_xkb_context);
}

void keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard,
                     uint32_t format, int32_t fd, uint32_t size) {

  // TODO: propagate error to thread manager to terminate all operations
  assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

  char *key_map = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  assert(key_map != MAP_FAILED);

  struct xkb_keymap *xkb_keymap = xkb_keymap_new_from_string(
      key_xkb_context, key_map, XKB_KEYMAP_FORMAT_TEXT_V1,
      XKB_KEYMAP_COMPILE_NO_FLAGS);

  munmap(key_map, size);
  close(fd);

  struct xkb_state *xkb_state = xkb_state_new(xkb_keymap);

  xkb_keymap_unref(key_xkb_keymap);
  xkb_state_unref(key_xkb_state);

  key_xkb_keymap = xkb_keymap;
  key_xkb_state = xkb_state;
};

void keyboard_enter(void *data, struct wl_keyboard *wl_keyboard,
                    uint32_t serial, struct wl_surface *surface,
                    struct wl_array *keys) {

  flexiwin_key_event *key_state = (flexiwin_key_event *)data;
  // key_state->event_type = flexiwin_key_enter;
};

void keyboard_leave(void *data, struct wl_keyboard *wl_keyboard,
                    uint32_t serial, struct wl_surface *surface) {
  flexiwin_key_event *key_state = (flexiwin_key_event *)data;
  key_state->event_type = flexiwin_key_leave;
};

void keyboard_key(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
                  uint32_t time, uint32_t key, uint32_t state) {

  flexiwin_key_event *key_state = (flexiwin_key_event *)data;
  xkb_keysym_t sym = xkb_state_key_get_one_sym(key_xkb_state, key + 8);
  key_state->serial = serial;
  key_state->key = sym;

  switch (state) {
  case WL_KEYBOARD_KEY_STATE_PRESSED:
    key_state->event_type = flexiwin_key_on_press;
    break;
  case WL_KEYBOARD_KEY_STATE_RELEASED:
    key_state->event_type = flexiwin_key_on_release;
    break;
  };
};

void keyboard_modifiers(void *data, struct wl_keyboard *wl_keyboard,
                        uint32_t serial, uint32_t mods_depressed,
                        uint32_t mods_latched, uint32_t mods_locked,
                        uint32_t group) {

  xkb_state_update_mask(key_xkb_state, mods_depressed, mods_latched,
                        mods_locked, 0, 0, group);
};

void keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard,
                          int32_t rate, int32_t delay) {};

const struct wl_keyboard_listener keyboard_listener = {
    .keymap = keyboard_keymap,
    .enter = keyboard_enter,
    .leave = keyboard_leave,
    .key = keyboard_key,
    .modifiers = keyboard_modifiers,
    .repeat_info = keyboard_repeat_info};

static void seat_capability_callback(void *data, struct wl_seat *wl_seat,
                                     uint32_t capabilities) {

  flexiwinState *bypass = (flexiwinState *)data;
  bool keyboardPresent = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

  if (keyboardPresent && bypass->display_keyboard == NULL) {
    bypass->display_keyboard = wl_seat_get_keyboard(wl_seat);
    wl_keyboard_add_listener(bypass->display_keyboard, &keyboard_listener,
                             &bypass->keyEvent);
  }

  if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
    bypass->display_pointer = wl_seat_get_pointer(wl_seat);
    wl_pointer_add_listener(bypass->display_pointer, &pointer_listener,
                            &bypass->pointerEvent);
  }
};

static void seat_name_callback(void *data, struct wl_seat *wl_seat,
                               const char *name) {
  std::cout << "[seat name] " << name << std::endl;
};

const struct wl_seat_listener wl_seat_obj = {
    .capabilities = seat_capability_callback, .name = seat_name_callback};

static void surface_pixel_format(void *data, wl_shm *wl_shm, uint32_t format) {
  switch (format) {
  case WL_SHM_FORMAT_ARGB8888:
    ((flexiwin_state *)data)->buffer_format = WL_SHM_FORMAT_ARGB8888;
    ((flexiwin_state *)data)->mask |= FLEXI_FORMAT_OK;
    break;
  };
};

const struct wl_shm_listener wl_shm_callback = {.format = surface_pixel_format};

static void wl_output_geometry(void *data, struct wl_output *wl_output,
                               int32_t x, int32_t y, int32_t physical_width,
                               int32_t physical_height, int32_t subpixel,
                               const char *make, const char *model,
                               int32_t transform) {};
static void wl_output_mode(void *data, struct wl_output *wl_output,
                           uint32_t flags, int32_t width, int32_t height,
                           int32_t refresh) {};
static void wl_output_done(void *data, struct wl_output *wl_output) {
  flexiwinState *info = (flexiwinState *)data;
  info->configured = true;
  std::cout << "[output done] " << std::endl;
};
static void wl_output_scale(void *data, struct wl_output *wl_output,
                            int32_t factor) {};
static void wl_output_name(void *data, struct wl_output *wl_output,
                           const char *name) {};
static void wl_output_description(void *data, struct wl_output *wl_output,
                                  const char *description) {};

const struct wl_output_listener display_output_listener = {
    .geometry = wl_output_geometry,
    .mode = wl_output_mode,
    .done = wl_output_done,
    .scale = wl_output_scale,
    .name = wl_output_name,
    .description = wl_output_description,
};
