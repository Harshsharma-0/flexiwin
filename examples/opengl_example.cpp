#include "flexiwin/flexiwin.hpp"
#include <EGL/egl.h>
#include <GLES2/gl2.h>

flexiwin_state wmstate = {0};
flexiwin_egl_info eglstate = {0};

const char *appname = "flexiwin opengl example";

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

int main() {

  eglstate.ctx_attribs = ctx_attr;
  eglstate.config_attribs = fb_attr;
  eglstate.num_cfg = 1;
  flexiwin_init(&wmstate, appname, 300, 300, window_type::win_dynamic);

  flexiwin_enable_gl(&wmstate, &eglstate);
  flexiwin_create(&wmstate, window_mode::customSize);
  flexiwin_create_gl_ctx(&wmstate);

  while (wmstate.mask & FLEXI_WINDOW_RUNNING) {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(eglstate.display, wmstate.surface);
    wl_display_dispatch(wmstate.display);
  };
  flexiwin_destroy_egl(&eglstate);
  flexiwin_destroy(&wmstate);
  return 0;
}
