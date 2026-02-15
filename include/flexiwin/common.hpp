#pragma once

#include "events/common.hpp"
#include "events/keyboard.hpp"
#include "events/mouse.hpp"
#include <EGL/egl.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>

constexpr int flexiwin_min_width = 250;
constexpr int flexiwin_min_height = 250;

typedef struct flexiwin_state {
  char *appname;

  struct wl_display *display;
  struct wl_surface *surface;
  struct wl_registry *registry;
  struct wl_shm *display_shm;
  struct wl_shm_pool *display_shm_pool;
  struct wl_compositor *display_compositor;
  struct wl_seat *display_seat;
  struct wl_output *display_output;
  struct wl_buffer *display_buffer;
  struct wl_keyboard *display_keyboard;
  struct wl_pointer *display_pointer;

  struct xdg_wm_base *display_xdg_base;
  struct xdg_surface *display_xdg_surface;
  struct xdg_toplevel *xdg_surface_toplevel;

  struct zxdg_output_v1 *xdg_output;
  struct zxdg_output_manager_v1 *xdg_output_manager;
  wl_shm_format buffer_format;

  struct display_info {
    int32_t width;
    int32_t height;
    int32_t stride;
    int32_t size;
    int32_t bound_height;
    int32_t bount_width;
  } display_info;

  struct local_info {
    int32_t width;
    int32_t height;
    int32_t stride;
    int32_t size;
  } local_info;

  flexiwin_egl_info *egl_info;
  window_resize_type resize_type;

  int shmFd;
  int displayFd;
  uint32_t evSerial;
  bool configured;

  size_t rawPixelSize;
  uint32_t *rawPixels;
  int winX, winY;

  size_t readyMask;

  flexiwin_key_event keyEvent;
  flexiwin_pointer_event pointerEvent;
} flexiwin_state;
typedef flexiwin_state flexiwinState;
