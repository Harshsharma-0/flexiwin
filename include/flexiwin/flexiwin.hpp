#pragma once
#ifndef __FLEXON_WAYLAND_WINDOWMANAGER__
#define __FLEXON_WAYLAND_WINDOWMANAGER__

#include "./wayland-callback.hpp"
#include "./xdg-output-client-protocol.h"
#include "./xdg-shell-client-protocol.h"
#include "flexiwin/wayland-callback.hpp"
#include "flexiwin/xdg/xdg.hpp"

#define FLEXI_DISPLAY_OK 1
#define FLEXI_REGISTRY_OK (1 << 1)
#define FLEXI_COMPOSITOR_OK (1 << 2)
#define FLEXI_SURFACE_OK (1 << 3)
#define FLEXI_XDG_SURFACE_OK (1 << 4)
#define FLEXI_XDG_TOPLEVEL_OK (1 << 5)
#define FLEXI_XDG_WM_BASE_OK (1 << 6)
#define FLEXI_WINDOW_RESIZED (1 << 7)
#define FLEXI_WINDOW_RUNNING (1 << 8)

namespace flexi {

constexpr int minWindowWidth = 250;
constexpr int minWindowHeight = 250;

struct pointer_event {

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

struct key_event {
  uint32_t event_type;
  uint32_t serial;
  uint32_t key;
};

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

  struct display_info {
    int32_t width;
    int32_t height;
    int32_t stride;
    int32_t size;
    int32_t bound_height;
    int32_t bount_width;
  } display_info;

  int32_t width;
  int32_t height;

  int shmFd;
  int displayFd;
  uint32_t evSerial;
  bool configured;

  size_t rawPixelSize;
  uint32_t *rawPixels;
  int winX, winY;

  size_t readyMask;
} WMState;

class windowManager {

public:
  windowManager() = default;
  ~windowManager() = default;
  int initlize(char *appname, int width, int height);
  int moveXY(int x, int y);
  int resize(int width, int height);
  int getWinFd() const;
  int destroy(int retVal = -1) noexcept;

private:
  static int allocateShm(WMState &state, int height, int width);
  WMState wmState = {0};
};
}; // namespace flexi
#endif
