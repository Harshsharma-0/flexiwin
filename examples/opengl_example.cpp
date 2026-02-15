#include "flexiwin/flexiwin.hpp"
#include <GLES2/gl2.h>
#include <iostream>
#include <wayland-client-core.h>

flexiwin_state wmstate = {0};
flexiwin_egl_info eglstate = {0};

const char *appname = "iuf";

int main() {

  EGLint fb_attr[] = {EGL_SURFACE_TYPE,
                      EGL_WINDOW_BIT,
                      EGL_RENDERABLE_TYPE,
                      EGL_OPENGL_ES2_BIT,
                      EGL_RED_SIZE,
                      8,
                      EGL_GREEN_SIZE,
                      8,
                      EGL_BLUE_SIZE,
                      8,
                      EGL_NONE};
  EGLint ctx_attr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE};
  eglstate.ctx_attribs = ctx_attr;
  eglstate.config_attribs = fb_attr;
  eglstate.num_cfg = 1;
  flexiwin_enable_gl(&wmstate, &eglstate);
  flexiwin_init(&wmstate, (char *)appname, 300, 300, window_type::win_dynamic);
  flexiwin_create(&wmstate, window_mode::customSize);
  flexiwin_create_gl_ctx(&wmstate);

  while (wmstate.readyMask & FLEXI_WINDOW_RUNNING) {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(eglstate.display, wmstate.surface);
    wl_display_dispatch(wmstate.display);
  };
  flexiwin_destroy(&wmstate, 0);
  return 0;
}
