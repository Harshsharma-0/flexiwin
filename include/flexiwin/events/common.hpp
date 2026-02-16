#pragma once

/*
 * macro not to be used publically
 */
#define FLEXI_XDG_CONFIGURED 1
#define FLEXI_FORMAT_OK (1 << 2)

/*
 * macro mask to check if the window was resized
 */
#define FLEXI_WINDOW_RESIZED (1 << 3)
/*
 * macro mask to check if the window is running or not
 */
#define FLEXI_WINDOW_RUNNING (1 << 4)

/*
 * macro to check if there any key event available
 */
#define FLEXI_KEY_EVENT (1 << 5)
/*
 * macro to check if there any mouse event available
 */
#define FLEXI_MOUSE_EVENT (1 << 6)

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
