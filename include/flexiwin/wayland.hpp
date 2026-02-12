#pragma once
#ifndef __FLEXON_WAYLAND_WINDOWMANAGER__
#define __FLEXON_WAYLAND_WINDOWMANAGER__

#include "./wayland-callback.hpp"
#include "./xdg-output.h"

namespace flexon {
constexpr int minWindowWidth = 250;
constexpr int minWindowHeight = 250;

typedef struct WMState {
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

  int width;
  int height;
  int shmFd;
  int displayFd;
  size_t rawPixelSize;
  uint32_t *rawPixels;
} WMState;

class windowManager {

public:
  windowManager() = default;
  ~windowManager() = default;
  int initlize(char *appname, int width, int height);
  int moveXY(int x, int y);
  int resize(int width, int height);

private:
  static int allocateShm(flexon::WMState &state, int height, int width);
  WMState wmState = {0};
};

}; // namespace flexon

#endif
