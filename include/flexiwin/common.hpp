#pragma once

#include <EGL/egl.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>

#define FLEXI_DISPLAY_OK 1
#define FLEXI_REGISTRY_OK (1 << 1)
#define FLEXI_COMPOSITOR_OK (1 << 2)
#define FLEXI_SURFACE_OK (1 << 3)
#define FLEXI_XDG_SURFACE_OK (1 << 4)
#define FLEXI_XDG_TOPLEVEL_OK (1 << 5)
#define FLEXI_XDG_WM_BASE_OK (1 << 6)
#define FLEXI_WINDOW_RESIZED (1 << 7)
#define FLEXI_WINDOW_RUNNING (1 << 8)
#define FLEXI_SHM_OK (1 << 9)
#define FLEXI_SHM_POOL_OK (1 << 10)
#define FLEXI_FROMAT_OK (1 << 11)
#define FLEXI_WIN_STATIC (1 << 12)

enum window_resize_type { win_none = 0, win_shrink, win_grow };

enum window_type {
  win_static,
  win_dynamic,
};
enum window_mode {
  fullScreen = 0,
  minimized,
  maximized,
  customSize,
  closed,
  focused,
};

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
  window_resize_type resize_type;
  int shmFd;
  int displayFd;
  uint32_t evSerial;
  bool configured;

  size_t rawPixelSize;
  uint32_t *rawPixels;
  int winX, winY;

  size_t readyMask;
} flexiwin_state;
typedef flexiwin_state flexiwinState;
