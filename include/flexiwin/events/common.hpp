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

#define FLEXI_WIN_MAXIMIZED (1 << 7)
#define FLEXI_WIN_MINIMIZED (1 << 8)
#define FLEXI_WIN_FULLSCREEN (1 << 9)

enum  { 
  window_resize_type_none = 0, 
  window_resize_type_shrink,
  window_resize_type_grow 
};

struct flexiwin_egl_info;

enum {
  window_type_static,
  window_type_dynamic,
};

enum  {
  window_mode_fullScreen = 0,
  window_mode_minimized,
  window_mode_maximized,
  window_mode_customSize,
  window_mode_closed,
  window_mode_focused,
};
