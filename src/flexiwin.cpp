#include "flexiwin/flexiwin.hpp"
#include "flexiwin/common.hpp"
#include "flexiwin/events/common.hpp"
#include "flexiwin/wayland-callback.hpp"
#include "flexiwin/xdg-output-client-protocol.h"
#include "flexiwin/xdg-shell-client-protocol.h"

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl-core.h>

static void randname(char *buf) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long r = ts.tv_nsec;
  for (int i = 0; i < 6; ++i) {
    buf[i] = 'A' + (r & 15) + (r & 16) * 2;
    r >>= 5;
  }
}

static void register_global(void *data, wl_registry *registry, uint32_t name,
                            const char *interface, uint32_t version) {

  flexiwin_state *bypass = (flexiwin_state *)data;
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    bypass->display_compositor = (wl_compositor *)wl_registry_bind(
        registry, name, &wl_compositor_interface, 6);
  }

  if (strcmp(interface, wl_seat_interface.name) == 0) {
    bypass->display_seat =
        (wl_seat *)wl_registry_bind(registry, name, &wl_seat_interface, 7);
    wl_seat_add_listener(bypass->display_seat, &wl_seat_obj, data);
  }

  if (strcmp(interface, wl_shm_interface.name) == 0) {
    bypass->display_shm =
        (wl_shm *)wl_registry_bind(registry, name, &wl_shm_interface, 2);
    wl_shm_add_listener(bypass->display_shm, &wl_shm_callback, data);
  }

  if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    bypass->display_xdg_base = (xdg_wm_base *)wl_registry_bind(
        registry, name, &xdg_wm_base_interface, 5);

    xdg_wm_base_add_listener(bypass->display_xdg_base,
                             &xdg_wm_base_callback_listener, data);
  };

  if (strcmp(interface, wl_output_interface.name) == 0) {
    bypass->display_output =
        (wl_output *)wl_registry_bind(registry, name, &wl_output_interface, 3);
    wl_output_add_listener(bypass->display_output, &display_output_listener,
                           data);
  }

  if (strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
    bypass->xdg_output_manager = (zxdg_output_manager_v1 *)wl_registry_bind(
        registry, name, &zxdg_output_manager_v1_interface, 3);
  }
}

static void remove_global(void *data, wl_registry *registry, uint32_t name) {
  std::cout << "destroyed" << std::endl;
};

static const struct wl_registry_listener display_registry_listener = {
    .global = register_global, .global_remove = remove_global};

static void add_xdg_listeners(flexiwin_state *state, int32_t max_width,
                              int32_t max_height) {

  xdg_surface_add_listener(state->display_xdg_surface,
                           &xdg_surface_callback_listener, state);
  xdg_toplevel_add_listener(state->xdg_surface_toplevel,
                            &xdg_surface_callback_listener_toplevel, state);

  xdg_toplevel_set_max_size(state->xdg_surface_toplevel, max_width, max_height);

  xdg_toplevel_set_min_size(state->xdg_surface_toplevel, flexiwin_min_width,
                            flexiwin_min_height);
};

static int create_start_wayland(flexiwin_state *state) {
  if (keyboard_init() != 0)
    return -1;

  if ((state->display = wl_display_connect(NULL)) == NULL)
    return -1;

  state->displayFd = wl_display_get_fd(state->display);

  if ((state->registry = wl_display_get_registry(state->display)) == NULL)
    return -1;

  wl_registry_add_listener(state->registry, &display_registry_listener, state);
  wl_display_roundtrip(state->display);
  state->xdg_output = zxdg_output_manager_v1_get_xdg_output(
      state->xdg_output_manager, state->display_output);

  zxdg_output_v1_add_listener(state->xdg_output, &xdg_output_listener, state);

  if (state->display_compositor == NULL)
    return -1;

  state->surface = wl_compositor_create_surface(state->display_compositor);

  if (state->surface == NULL)
    return -1;

  state->display_xdg_surface =
      xdg_wm_base_get_xdg_surface(state->display_xdg_base, state->surface);

  if (state->display_xdg_surface == NULL)
    return -1;

  state->xdg_surface_toplevel =
      xdg_surface_get_toplevel(state->display_xdg_surface);

  if (state->xdg_surface_toplevel == NULL)
    return -1;

  wl_surface_commit(state->surface);

  while (wl_display_roundtrip(state->display) != -1 && !state->configured) {
  }
  if (state->display_xdg_base == NULL)
    return -1;

  return 0;
}

static int create_egl_window(flexiwin_state *state, int width, int height) {
  wl_egl_window *egl_window =
      wl_egl_window_create(state->surface, width, height);
  if (egl_window == NULL)
    return -1;

  int max_width = state->display_info.width;
  int max_height = state->display_info.height;

  add_xdg_listeners(state, max_width, max_height);

  state->egl_info->window = egl_window;
  return 0;
};

int flexiwin_init(flexiwin_state *state, const char *appname, int width,
                  int height, int window_type) {

  if (width < flexiwin_min_width || height < flexiwin_min_height)
    return -1;

  memset(state, 0, sizeof(flexiwin_state));
  if (appname) {
    size_t len = strlen(appname);
    if ((state->appname = new (std::nothrow) char[len + 1]) == nullptr)
      return -1;

    memcpy(state->appname, appname, len);
    state->appname[len] = '\0';
  }

  state->local_info.width = width;
  state->local_info.height = height;
  state->resize_type = window_resize_type_none;
  state->win_type = window_type;

  if (create_start_wayland(state) != 0)
    return -1;
  return 0;
};

int flexiwin_create(flexiwin_state *state) {

  int32_t width = state->local_info.width;
  int32_t height = state->local_info.height;

  if (state->egl_info == NULL)
    return -1;

  if (create_egl_window(state, width, height) != 0)
    return -1;

  if (state->appname != nullptr)
    xdg_toplevel_set_title(state->xdg_surface_toplevel, state->appname);

  while (wl_display_roundtrip(state->display) != -1 &&
         (state->mask & FLEXI_XDG_CONFIGURED)) {
  }
  state->mask |= FLEXI_WINDOW_RUNNING;
  return 0;
};
int flexiwin_create_gl_ctx(flexiwin_state *state) {
  if (state->egl_info == NULL || state->display == NULL) {
    std::cout << "[egl ctx error] gl not enabled or display not ok"
              << std::endl;

    return -1;
  }

  flexiwin_egl_info *egl_info = state->egl_info;
  EGLDisplay display = eglGetDisplay((EGLNativeDisplayType)state->display);

  if (display == EGL_NO_DISPLAY) {
    std::cout << "[egl ctx error] failed to get display" << std::endl;

    return -1;
  }

  if (!eglInitialize(display, &egl_info->major_ver, &egl_info->major_ver)) {
    std::cout << "[egl ctx error] failed to get version" << std::endl;
    return -1;
  }

  EGLint num_cfg = 0;
  if ((eglGetConfigs(display, NULL, 0, &num_cfg) != EGL_TRUE) ||
      (num_cfg == 0)) {
    std::cout << "[egl ctx error] failed to get cfg" << std::endl;

    return -1;
  }

  if (num_cfg < egl_info->num_cfg) {
    return -1;
  }

  if ((eglChooseConfig(display, egl_info->config_attribs, &egl_info->config,
                       egl_info->num_cfg, &num_cfg) != EGL_TRUE) ||
      (num_cfg != egl_info->num_cfg)) {
    std::cout << "[egl] config choose error " << num_cfg << std::endl;
    return -1;
  }

  state->surface = (wl_surface *)eglCreateWindowSurface(
      display, egl_info->config, (EGLNativeWindowType)egl_info->window, NULL);

  if (state->surface == EGL_NO_SURFACE)
    return -1;

  egl_info->surface = state->surface;
  egl_info->context = eglCreateContext(display, egl_info->config,
                                       EGL_NO_CONTEXT, egl_info->ctx_attribs);
  if (egl_info->context == EGL_NO_CONTEXT) {
    std::cout << "[egl ctx error] failed to create ctx" << std::endl;
    return -1;
  };
  if (!eglMakeCurrent(display, state->surface, state->surface,
                      egl_info->context))
    return -1;

  egl_info->display = display;

  return 0;
};

void flexiwin_destroy_egl(flexiwin_egl_info *egl_info) {
  if (egl_info->window != NULL)
    eglDestroySurface(egl_info->display, egl_info->surface);

  if (egl_info->display != EGL_NO_DISPLAY)
    wl_egl_window_destroy(egl_info->window);

  if (egl_info->context != EGL_NO_CONTEXT)
    eglDestroyContext(egl_info->display, egl_info->context);
}
static void flexiwin_destroy_xdg(flexiwin_state *state) {
  if (state->xdg_surface_toplevel)
    xdg_toplevel_destroy(state->xdg_surface_toplevel);

  if (state->display_xdg_surface)
    xdg_surface_destroy(state->display_xdg_surface);

  if (state->display_xdg_base)
    xdg_wm_base_destroy(state->display_xdg_base);
};

void flexiwin_destroy(flexiwinState *state) {

  if (state->display_shm_pool)
    wl_shm_pool_destroy(state->display_shm_pool);

  flexiwin_destroy_xdg(state);
  if (state->surface && state->egl_info == NULL)
    wl_surface_destroy(state->surface);

  if (state->display_compositor)
    wl_compositor_destroy(state->display_compositor);

  if (state->registry)
    wl_registry_destroy(state->registry);

  if (state->display)
    wl_display_disconnect(state->display);

  if (state->appname != NULL)
    delete state->appname;

  keyboard_destroy();
};
