#pragma once
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <wayland-egl-core.h>
#pragma once

#include "./common.hpp"
#include "./wayland-callback.hpp"
#include "./xdg-output-client-protocol.h"
#include "./xdg-shell-client-protocol.h"
#include "./xdg/xdg.hpp"
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
int flexiwin_init(flexiwin_state *state, char *appname, int width, int height,
                  window_type type);
int flexiwin_create(flexiwin_state *state, window_mode mode);
int flexiwin_create_gl_ctx(flexiwin_state *state);
int flexiwin_moveXY(int x, int y);
int flexiwin_resize(int width, int height);
int flexiwin_get_win_fd();
void flexiwin_destroy_egl(flexiwin_egl_info *egl_info);
void flexiwin_destroy(flexiwin_state *state);
