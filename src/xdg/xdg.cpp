#include "flexiwin/xdg/xdg.hpp"
#include "flexiwin/wayland.hpp"
#include "flexiwin/xdg-shell-client-protocol.h"

#include <iostream>

/*
static void xdg_wm_base_ping_callback(void *data,
                                      struct xdg_wm_base *xdg_wm_base,
                                      uint32_t serial) {

  xdg_wm_base_pong(xdg_wm_base, serial);
}

const struct xdg_wm_base_listener xdg_wm_base_callback_listener = {
    .ping = xdg_wm_base_ping_callback};

static void xdg_surface_callback_configure(void *data,
                                           struct xdg_surface *xdg_surface,
                                           uint32_t serial) {
  struct window_state *info = (window_state *)data;
  xdg_surface_ack_configure(xdg_surface, serial);
  info->configuredxdg = true;
}

const struct xdg_surface_listener xdg_surface_callback_listener = {
    .configure = xdg_surface_callback_configure};

static void xdg_surface_callback_toplevel_configure(
    void *data, struct xdg_toplevel *xdg_toplevel, int32_t width,
    int32_t height, struct wl_array *states) {
  struct window_state *info = (window_state *)data;

  int size = width * height * sizeof(uint32_t);
  int rsize = info->rsize;

  if (rsize > size)
    info->fresize = WINDOW_RESIZE_SHRINK;

  if (rsize == size)
    info->fresize = WINDOW_ESCAPE;

  if (rsize < size)
    info->fresize = WINDOW_RESIZE_GROW;

  info->resized = true;
  info->dwidth = width;
  info->dheight = height;
}

static void
xdg_surface_callback_toplevel_close(void *data,
                                    struct xdg_toplevel *xdg_toplevel) {
  struct window_state *wayland_config = (window_state *)data;
  wayland_config->running = false;
}
static void xdg_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel,
                                 int32_t width, int32_t height) {
  struct window_state *info = (struct window_state *)data;
  info->width_bound = width;
  info->height_bound = height;
  info->configuredxdg = true;
}

const struct xdg_toplevel_listener xdg_surface_callback_listener_toplevel = {
    .configure = xdg_surface_callback_toplevel_configure,
    .close = xdg_surface_callback_toplevel_close,
    .configure_bounds = xdg_configure_bounds};

static void xdg_output_logical_position(void *data,
                                        struct zxdg_output_v1 *zxdg_output_v1,
                                        int32_t x, int32_t y) {};

static void xdg_output_logical_size(void *data,
                                    struct zxdg_output_v1 *zxdg_output_v1,
                                    int32_t width, int32_t height) {

  struct window_state *info = (struct window_state *)data;
  info->display_height = height;
  info->display_width = width;
  info->stride = width * sizeof(uint32_t);
  info->size = info->stride * height;
};

static void xdg_output_done(void *data, struct zxdg_output_v1 *zxdg_output_v1) {
  std::cout << "[xdg done]" << std::endl;
};

static void xdg_output_name(void *data, struct zxdg_output_v1 *zxdg_output_v1,
                            const char *name) {
  std::cout << "[xdg name]" << name << std::endl;
};
static void xdg_output_description(void *data,
                                   struct zxdg_output_v1 *zxdg_output_v1,
                                   const char *description) {
  std::cout << "[xdg description]" << description << std::endl;
};

const struct zxdg_output_v1_listener xdg_output_listener = {
    .logical_position = xdg_output_logical_position,
    .logical_size = xdg_output_logical_size,
    .done = xdg_output_done,
    .name = xdg_output_name,
    .description = xdg_output_description,
};
*/
