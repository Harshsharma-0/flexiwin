#pragma once

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
#define FLEXI_KEY_EVENT (1 << 11)
#define FLEXI_MOUSE_EVENT (1 << 12)
#define FLEXI_WIN_EGL_OK (1 << 13)
#define FLEXI_WIN_EGL_ENABLE (1 << 14)
#define FLEXI_XDG_CONFIGURED (1 << 15)

enum window_resize_type { win_none = 0, win_shrink, win_grow };

struct flexiwin_egl_info;
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
