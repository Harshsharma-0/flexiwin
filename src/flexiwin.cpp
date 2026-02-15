#include "flexiwin/flexiwin.hpp"
#include "flexiwin/common.hpp"

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
    if (bypass->display_xdg_base == NULL)
      return;

    bypass->readyMask |= FLEXI_XDG_WM_BASE_OK;
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
    bypass->xdg_output = zxdg_output_manager_v1_get_xdg_output(
        bypass->xdg_output_manager, bypass->display_output);
    zxdg_output_v1_add_listener(bypass->xdg_output, &xdg_output_listener, data);
  }
}

static void remove_global(void *data, wl_registry *registry, uint32_t name) {
  std::cout << "destroyed" << std::endl;
};

static const struct wl_registry_listener display_registry_listener = {
    .global = register_global, .global_remove = remove_global};

static int allocateShm(flexiwin_state *state, int width, int height,
                       int tmp_size) {

  if (width < flexiwin_min_width || height < flexiwin_min_height)
    return -1;

  char name[] = "/wl_shm-XXXXXX";
  randname(name + sizeof(name) - 7);

  state->shmFd = -1;
  if ((state->shmFd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600)) < 0) {
    std::cout << "[SHM] Creation failed : " << std::endl;
    return -1;
  }

  shm_unlink(name);
  if (ftruncate(state->shmFd, tmp_size) < 0) {
    std::cout << "FD failed" << std::endl;
    close(state->shmFd);
    return -1;
  }

  state->rawPixelSize = sizeof(uint32_t) * width * height;
  if ((state->rawPixels =
           (uint32_t *)mmap(NULL, state->rawPixelSize, PROT_READ | PROT_WRITE,
                            MAP_SHARED, state->shmFd, 0)) == MAP_FAILED) {
    std::cout << "mapping failed " << std::endl;
    close(state->shmFd);
    return -1;
  };
  return 0;
};

static void add_xdg_listeners(flexiwin_state *state, int32_t tmp_width,
                              int32_t tmp_height) {

  xdg_surface_add_listener(state->display_xdg_surface,
                           &xdg_surface_callback_listener, state);
  xdg_toplevel_add_listener(state->xdg_surface_toplevel,
                            &xdg_surface_callback_listener_toplevel, state);

  xdg_toplevel_set_max_size(state->xdg_surface_toplevel, tmp_height, tmp_width);

  xdg_toplevel_set_min_size(state->xdg_surface_toplevel, flexiwin_min_width,
                            flexiwin_min_height);
};

static int create_start_wayland(flexiwin_state *state) {
  if (keyboard_init() != 0)
    return -1;

  if ((state->display = wl_display_connect(NULL)) == NULL)
    return -1;

  state->displayFd = wl_display_get_fd(state->display);
  state->readyMask |= FLEXI_DISPLAY_OK;

  if ((state->registry = wl_display_get_registry(state->display)) == NULL)
    return flexiwin_destroy(state, -1);

  state->readyMask |= FLEXI_REGISTRY_OK;

  wl_registry_add_listener(state->registry, &display_registry_listener, state);
  wl_display_roundtrip(state->display);

  if (state->display_compositor == NULL)
    return flexiwin_destroy(state, -1);

  state->readyMask |= FLEXI_COMPOSITOR_OK;

  state->surface = wl_compositor_create_surface(state->display_compositor);

  if (state->surface == NULL)
    return flexiwin_destroy(state, -1);

  state->readyMask |= FLEXI_SURFACE_OK;
  state->display_xdg_surface =
      xdg_wm_base_get_xdg_surface(state->display_xdg_base, state->surface);

  if (state->display_xdg_surface == NULL)
    return flexiwin_destroy(state, -1);

  state->readyMask |= FLEXI_XDG_SURFACE_OK;
  state->xdg_surface_toplevel =
      xdg_surface_get_toplevel(state->display_xdg_surface);

  if (state->xdg_surface_toplevel == NULL)
    return flexiwin_destroy(state, -1);
  state->readyMask |= FLEXI_XDG_TOPLEVEL_OK;

  wl_surface_commit(state->surface);

  while (wl_display_roundtrip(state->display) != -1 && !state->configured) {
  }
  if (!(state->readyMask & FLEXI_XDG_WM_BASE_OK) ||
      !(state->readyMask & FLEXI_FROMAT_OK))
    return flexiwin_destroy(state, -1);

  return 0;
}
static int create_buffered_win(flexiwin_state *state, int width, int height,
                               window_type type) {
  int tmp_width = type == win_static ? width : state->display_info.width;
  int tmp_height = type == win_static ? height : state->display_info.height;
  int tmp_size =
      type != win_static ? state->display_info.size : state->local_info.size;

  if (allocateShm(state, tmp_width, tmp_height, tmp_size) < 0)
    return flexiwin_destroy(state, -1);

  state->readyMask |= FLEXI_SHM_OK;

  state->display_shm_pool =
      wl_shm_create_pool(state->display_shm, state->shmFd, tmp_size);
  add_xdg_listeners(state, tmp_width, tmp_height);

  if (state->display_shm_pool == NULL)
    return flexiwin_destroy(state, -1);

  state->readyMask |= FLEXI_SHM_POOL_OK;

  if (type != window_type::win_static) {
    tmp_width = width;
    tmp_height = height;
  }
  state->display_buffer = wl_shm_pool_create_buffer(
      state->display_shm_pool, 0, tmp_width, tmp_height,
      state->local_info.stride, state->buffer_format);

  wl_surface_attach(state->surface, state->display_buffer, 0, 0);
  wl_surface_commit(state->surface);
  return 0;
};

static int create_egl_window(flexiwin_state *state, int width, int height) {
  wl_egl_window *egl_window =
      wl_egl_window_create(state->surface, width, height);
  if (egl_window == NULL)
    return -1;

  add_xdg_listeners(state, width, height);

  state->egl_info->window = egl_window;
  return 0;
};

int flexiwin_init(flexiwin_state *state, char *appname, int width, int height,
                  window_type type) {

  if (width < flexiwin_min_width || height < flexiwin_min_height)
    return -1;

  size_t len = strlen(appname);
  state->appname = nullptr;
  if ((state->appname = new (std::nothrow) char[len + 1]) == nullptr) {
    return -1;
  };

  state->local_info.width = width;
  state->local_info.height = height;
  state->local_info.stride = width * sizeof(uint32_t);
  state->local_info.size = width * height * sizeof(uint32_t);
  state->resize_type = window_resize_type::win_none;
  if (type == window_type::win_static)
    state->readyMask |= FLEXI_WIN_STATIC;

  memcpy(state->appname, appname, len);
  state->appname[len] = '\0';

  if (create_start_wayland(state) != 0)
    return -1;
  return 0;
};

int flexiwin_create(flexiwin_state *state, window_mode mode) {

  int32_t width = state->local_info.width;
  int32_t height = state->local_info.height;
  window_type type = (state->readyMask & FLEXI_WIN_STATIC)
                         ? window_type::win_static
                         : window_type::win_dynamic;

  if (state->readyMask & FLEXI_WIN_EGL_ENABLE) {
    if (create_egl_window(state, width, height) != 0)
      return -1;
  } else {
    if (create_buffered_win(state, width, height, type) != 0)
      return -1;
  };
  while (wl_display_roundtrip(state->display) != -1 &&
         (state->readyMask & FLEXI_XDG_CONFIGURED)) {
  }
  state->readyMask |= FLEXI_WINDOW_RUNNING;
  return 0;
};
int flexiwin_create_gl_ctx(flexiwin_state *state) {
  if (!(state->readyMask & FLEXI_WIN_EGL_ENABLE) ||
      !(state->readyMask & FLEXI_DISPLAY_OK)) {
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
    std::cout << "ga" << std::endl;
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
  state->readyMask |= FLEXI_WIN_EGL_OK;

  return 0;
};
int flexiwin_moveXY(int x, int y) { return 0; };
int flexiwin_resize(int width, int height) { return 0; };

int flexiwin_get_win_fd(flexiwinState *state) { return state->displayFd; }
int flexiwin_destroy(flexiwinState *state, int retVal) {
  size_t mask = state->readyMask;

  if (mask & FLEXI_SHM_POOL_OK)
    wl_shm_pool_destroy(state->display_shm_pool);

  if (mask & FLEXI_SHM_OK) {
    close(state->shmFd);
    munmap(state->rawPixels, state->rawPixelSize);
  };
  if (mask & FLEXI_XDG_TOPLEVEL_OK)
    xdg_toplevel_destroy(state->xdg_surface_toplevel);

  if (mask & FLEXI_XDG_SURFACE_OK)
    xdg_surface_destroy(state->display_xdg_surface);

  if (mask & FLEXI_XDG_WM_BASE_OK)
    xdg_wm_base_destroy(state->display_xdg_base);

  if (mask & FLEXI_WIN_EGL_ENABLE) {
    if (mask & FLEXI_WIN_EGL_ENABLE) {
      eglDestroySurface(state->egl_info->display, state->surface);
      wl_egl_window_destroy(state->egl_info->window);
    };
  } else {
    if (mask & FLEXI_SURFACE_OK) {
      wl_surface_destroy(state->surface);
    }
  };

  if (mask & FLEXI_WIN_EGL_ENABLE) {
    if (mask & FLEXI_WIN_EGL_ENABLE) {
      eglDestroyContext(state->egl_info->display, state->egl_info->context);
    };
  };

  if (mask & FLEXI_COMPOSITOR_OK)
    wl_compositor_destroy(state->display_compositor);

  if (mask & FLEXI_REGISTRY_OK)
    wl_registry_destroy(state->registry);

  if (mask & FLEXI_DISPLAY_OK)
    wl_display_disconnect(state->display);

  return retVal;
}
