#pragma once

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <wayland-egl-core.h>

#include "./common.hpp"
#include "./wayland-callback.hpp"
#include "./xdg-output-client-protocol.h"
#include "./xdg-shell-client-protocol.h"
#include "./xdg/xdg.hpp"
#include "flexiwin/events/common.hpp"
#include "flexiwin/events/keyboard.hpp"
#include "flexiwin/events/mouse.hpp"
#include <GLES2/gl2.h>
#include <cstdint>

typedef struct flexiwin_egl_info {
  wl_egl_window *window;
  wl_surface *surface;
  EGLint *ctx_attribs;
  EGLint *config_attribs;

  EGLDisplay display;
  EGLint major_ver;
  EGLint minor_ver;
  EGLContext context;
  EGLConfig config;
  EGLint num_cfg;
} flexiwin_egl_info;

inline void flexiwin_enable_gl(flexiwin_state *state,
                               flexiwin_egl_info *egl_info) {
  egl_info->window = NULL;
  egl_info->context = EGL_NO_CONTEXT;
  egl_info->display = EGL_NO_DISPLAY;
  egl_info->surface = (wl_surface *)EGL_NO_SURFACE;
  state->egl_info = egl_info;
};

inline int flexiwin_get_win_fd(flexiwinState *state) {
  return state->displayFd;
};

#define FLEXIWIN_SET_CHECK(typechk, type, rval)                                \
  if (typechk(state->mask & type) && state->xdg_surface_toplevel != NULL &&    \
      state->win_type != window_type::win_static)                              \
  rval

inline int flexiwin_set_maximized(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(!, FLEXI_WIN_MAXIMIZED, {
    xdg_toplevel_set_maximized(state->xdg_surface_toplevel);
    state->mask |= FLEXI_WIN_MAXIMIZED;
    return 0;
  })
  return -1;
};
inline int flexiwin_set_minimized(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(!, FLEXI_WIN_MINIMIZED, {
    xdg_toplevel_set_minimized(state->xdg_surface_toplevel);
    state->mask |= FLEXI_WIN_MINIMIZED;
    return 0;
  })
  return -1;
};
inline int flexiwin_set_fullscreen(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(!, FLEXI_WIN_FULLSCREEN, {
    xdg_toplevel_set_fullscreen(state->xdg_surface_toplevel,
                                state->display_output);
    state->mask |= FLEXI_WIN_FULLSCREEN;
    return 0;
  })
  return -1;
};

inline int flexiwin_unset_fullscreen(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(, FLEXI_WIN_FULLSCREEN, {
    xdg_toplevel_unset_fullscreen(state->xdg_surface_toplevel);
    state->mask &= ~FLEXI_WIN_FULLSCREEN;
    return 0;
  })
  return -1;
}
inline int flexiwin_unset_maximized(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(, FLEXI_WIN_MAXIMIZED, {
    xdg_toplevel_unset_maximized(state->xdg_surface_toplevel);
    state->mask &= ~FLEXI_WIN_MAXIMIZED;
    return 0;
  })
  return -1;
};
#undef FLEXIWIN_SET_CHECK

inline void flexiwin_moveXY(flexiwin_state *state, uint32_t serial) {
  if (state->xdg_surface_toplevel != NULL)
    xdg_toplevel_move(state->xdg_surface_toplevel, state->display_seat, serial);
};

/**
 * Here parameter edge is the xdg-egde enum for the specific edge to resize from
 * Example : flexiwin_resize(&state,serial,XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM);
 *    To resize from the bottom edge
 */

inline void flexiwin_resize(flexiwin_state *state, uint32_t serial,
                            uint32_t edge) {
  if (state->xdg_surface_toplevel != NULL)
    xdg_toplevel_resize(state->xdg_surface_toplevel, state->display_seat,
                        serial, edge);
};

inline void flexiwin_show_menu(flexiwin_state *state, uint32_t serial, int x,
                               int y) {
  if (state->xdg_surface_toplevel != NULL)
    xdg_toplevel_show_window_menu(state->xdg_surface_toplevel,
                                  state->display_seat, serial, x, y);
};

inline flexiwin_key_event flexiwin_get_key_event(flexiwin_state *state) {
  return state->keyEvent;
};

inline void flexiwin_add_pointer_callback(flexiwin_state *state,
                                          pointer_frame_cb callback) {

  state->pointerEvent.pointer_cb = callback;
};
int flexiwin_init(flexiwin_state *state, const char *appname, int width,
                  int height, window_type type);
int flexiwin_create(flexiwin_state *state, window_mode mode);
int flexiwin_create_gl_ctx(flexiwin_state *state);
void flexiwin_destroy_egl(flexiwin_egl_info *egl_info);
void flexiwin_destroy(flexiwin_state *state);
