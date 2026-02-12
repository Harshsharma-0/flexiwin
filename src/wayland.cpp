#include "flexiwin/wayland.hpp"
#include "flexiwin/wayland-callback.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>

static void randname(char *buf) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long r = ts.tv_nsec;
  for (int i = 0; i < 6; ++i) {
    buf[i] = 'A' + (r & 15) + (r & 16) * 2;
    r >>= 5;
  }
}

namespace flexon {

int windowManager::allocateShm(flexon::WMState &state, int width, int height) {

  if (flexon::minWindowWidth < 250 || flexon::minWindowHeight <= 250)
    return -1;

  char name[] = "/wl_shm-XXXXXX";
  randname(name + sizeof(name) - 7);

  state.shmFd = -1;
  if ((state.shmFd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600)) < 0) {
    std::cout << "[SHM] Creation failed : " << std::endl;
    return -1;
  }

  shm_unlink(name);
  if (ftruncate(state.shmFd, state.rawPixelSize) < 0) {
    std::cout << "FD failed" << std::endl;
    close(state.shmFd);
    return -1;
  }
  state.rawPixelSize = sizeof(uint32_t) * width * height;
  if ((state.rawPixels =
           (uint32_t *)mmap(NULL, state.rawPixelSize, PROT_READ | PROT_WRITE,
                            MAP_SHARED, state.shmFd, 0)) == MAP_FAILED) {
    std::cout << "mapping failed " << std::endl;
    close(state.shmFd);
    return -1;
  };

  return 0;
};

int windowManager::initlize(char *appname, int width, int height) {

  size_t len = strlen(appname);
  wmState.appname = nullptr;
  if ((wmState.appname = new (std::nothrow) char[len + 1]) == nullptr) {
    return -1;
  };

  if (keyboard_init() != 0)
    return -1;

  wmState.display = wl_display_connect(NULL);
  wmState.displayFd = wl_display_get_fd(wmState.display);
  wmState.registry = wl_display_get_registry(wmState.display);

  return 0;
};
int windowManager::moveXY(int x, int y) { return 0; };
int windowManager::resize(int width, int height) { return 0; };

}; // namespace flexon

/*

static void register_global(void *data, wl_registry *registry, uint32_t name,
                            const char *interface, uint32_t version) {

  struct window_state *bypass = (window_state *)data;
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
  }

  if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    bypass->display_xdg_base = (xdg_wm_base *)wl_registry_bind(
        registry, name, &xdg_wm_base_interface, 5);
    xdg_wm_base_add_listener(bypass->display_xdg_base,
                             &xdg_wm_base_callback_listener, data);
  }

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

bool waylandWM::bound(window &win) {
  if (win.height > 500 && win.width > 500 &&
      win.height < wmconfig.height_bound && win.width < wmconfig.width_bound)
    return true;

  return false;
};

/*
waylandWM::waylandWM(window &window) {

  if (keyboard_init() != 0) {
    std::cout << "[PANIC] Cannot create a keyboard system";
    return;
  }

  wmconfig.app_name = window.name;
  wmconfig.display = wl_display_connect(NULL);
  wmconfig.display_fd = wl_display_get_fd(wmconfig.display);
  wmconfig.registry = wl_display_get_registry(wmconfig.display);

  wl_registry_add_listener(wmconfig.registry, &display_registry_listener,
                           &wmconfig);
  wl_display_roundtrip(wmconfig.display);

  if (wmconfig.display == NULL || wmconfig.display_compositor == NULL) {
    std::cout << "cannot connect to wayland display" << std::endl;
    return;
  }

  wmconfig.surface = wl_compositor_create_surface(wmconfig.display_compositor);

  if (wmconfig.surface == NULL) {
    std::cout << "cannot create wayland surface" << std::endl;
    return;
  }

  wmconfig.display_xdg_surface =
      xdg_wm_base_get_xdg_surface(wmconfig.display_xdg_base, wmconfig.surface);
  wmconfig.xdg_surface_toplevel =
      xdg_surface_get_toplevel(wmconfig.display_xdg_surface);

  wl_surface_commit(wmconfig.surface);
  while (wl_display_dispatch(wmconfig.display) != -1 && !wmconfig.configured)
    ;

  if (allocate_shm(&wmconfig) == -1) {
    std::cout << "allocation failed" << std::endl;
  };

  xdg_surface_add_listener(wmconfig.display_xdg_surface,
                           &xdg_surface_callback_listener, &wmconfig);
  xdg_toplevel_add_listener(wmconfig.xdg_surface_toplevel,
                            &xdg_surface_callback_listener_toplevel, &wmconfig);

  xdg_toplevel_set_max_size(wmconfig.xdg_surface_toplevel,
                            wmconfig.display_width, wmconfig.display_height);

  xdg_toplevel_set_title(wmconfig.xdg_surface_toplevel, window.name);
  xdg_toplevel_set_app_id(wmconfig.xdg_surface_toplevel, window.name);
  xdg_toplevel_set_min_size(wmconfig.xdg_surface_toplevel, 200, 200);

  wmconfig.display_shm_pool =
      wl_shm_create_pool(wmconfig.display_shm, wmconfig.shm_fd, 10);
  wmconfig.display_buffer = wl_shm_pool_create_buffer(
      wmconfig.display_shm_pool, 0, 1, 1, 1, WL_SHM_FORMAT_ABGR8888);
  wl_surface_attach(wmconfig.surface, wmconfig.display_buffer, 0, 0);
  wl_surface_commit(wmconfig.surface);

  while (wl_display_dispatch(wmconfig.display) != -1 && !wmconfig.configuredxdg)
    ;

  switch (window.flag) {
  case WINDOW_FULL_SCREEN:
    xdg_toplevel_set_fullscreen(wmconfig.xdg_surface_toplevel,
                                wmconfig.display_output);
    wmconfig.windowstate = WINDOW_STATE_MAXIMIZED_FULL;
    break;
  case WINDOW_CUSTOM_SIZE:
    if (bound(window)) {
      wmconfig.dwidth = window.width;
      wmconfig.dheight = window.height;
      wmconfig.fresize = WINDOW_RESIZE_GROW;
      wmconfig.resize(&wmconfig);
      break;
    }

  case WINDOW_FULL_TOPBAR_SHOWN:
    xdg_toplevel_set_maximized(wmconfig.xdg_surface_toplevel);
    wmconfig.windowstate = WINDOW_STATE_MAXIMIZED_BOUNDED;
    break;
  };

  while (wl_display_dispatch(wmconfig.display) != -1 && !wmconfig.maxconfig) {
    if (wmconfig.resized == true) {
      wmconfig.resize(&wmconfig);
      wmconfig.resized = false;
    };
  }

  return;
}

void waylandWM::destroy() {
  close(wmconfig.shm_fd);
  munmap(wmconfig.pixels, wmconfig.size);
  wl_shm_pool_destroy(wmconfig.display_shm_pool);
  xdg_toplevel_destroy(wmconfig.xdg_surface_toplevel);
  xdg_surface_destroy(wmconfig.display_xdg_surface);
  wl_surface_destroy(wmconfig.surface);
  xdg_wm_base_destroy(wmconfig.display_xdg_base);
  wl_compositor_destroy(wmconfig.display_compositor);
  wl_registry_destroy(wmconfig.registry);
  wl_display_disconnect(wmconfig.display);

  return;
}
*/
