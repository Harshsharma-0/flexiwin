#include "flexiwin/xdg/xdg.hpp"
#include "flexiwin/flexiwin.hpp"
#include "flexiwin/xdg-shell-client-protocol.h"

#include <iostream>

static void xdg_wm_base_ping_callback(void *data,
                                      struct xdg_wm_base *xdg_wm_base,
                                      uint32_t serial) {

  ((flexiwinState *)data)->evSerial = serial;

  xdg_wm_base_pong(xdg_wm_base, serial);
}

const struct xdg_wm_base_listener xdg_wm_base_callback_listener = {
    .ping = xdg_wm_base_ping_callback};

static void xdg_surface_callback_configure(void *data,
                                           struct xdg_surface *xdg_surface,
                                           uint32_t serial) {
  xdg_surface_ack_configure(xdg_surface, serial);
  ((flexiwinState *)data)->configured = true;
}

const struct xdg_surface_listener xdg_surface_callback_listener = {
    .configure = xdg_surface_callback_configure};

static void xdg_surface_callback_toplevel_configure(
    void *data, struct xdg_toplevel *xdg_toplevel, int32_t width,
    int32_t height, struct wl_array *states) {

  flexiwinState *info = (flexiwinState *)data;

  if (info->readyMask & FLEXI_WIN_STATIC)
    return;

  int mask = 0;
  uint32_t size = width * height * sizeof(uint32_t);
  if (width > 0 && height > 0) {
    if (info->local_info.width != width) {
      info->local_info.width = width;
      info->local_info.stride = width * sizeof(uint32_t);
      mask = FLEXI_WINDOW_RESIZED;
    };

    if (info->local_info.height != height) {
      mask = FLEXI_WINDOW_RESIZED;
      info->local_info.height = height;
    }

    if (mask == FLEXI_WINDOW_RESIZED) {
      wl_buffer_destroy(info->display_buffer);
      info->display_buffer = info->display_buffer = wl_shm_pool_create_buffer(
          info->display_shm_pool, 0, width, height, info->local_info.stride,
          info->buffer_format);
      wl_surface_attach(info->surface, info->display_buffer, 0, 0);
      wl_surface_commit(info->surface);

      info->local_info.size = size;
      info->readyMask |= mask;
    }
  }
  // TODO: handle Resize Event
}

static void
xdg_surface_callback_toplevel_close(void *data,
                                    struct xdg_toplevel *xdg_toplevel) {
  ((flexiwinState *)data)->readyMask &= ~(FLEXI_WINDOW_RUNNING);
};

static void xdg_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel,
                                 int32_t width, int32_t height) {
  flexiwinState *info = (flexiwinState *)data;
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

  flexiwinState *info = (flexiwinState *)data;
  info->display_info.height = height;
  info->display_info.width = width;
  info->display_info.stride = width * sizeof(uint32_t);
  info->display_info.size = info->display_info.stride * height;
  std::cout << "[xdg size] " << width << " : " << height << std::endl;
};

static void xdg_output_done(void *data, struct zxdg_output_v1 *zxdg_output_v1) {
  std::cout << "[xdg done] " << std::endl;
};

static void xdg_output_name(void *data, struct zxdg_output_v1 *zxdg_output_v1,
                            const char *name) {
  std::cout << "[xdg name] " << name << std::endl;
};
static void xdg_output_description(void *data,
                                   struct zxdg_output_v1 *zxdg_output_v1,
                                   const char *description) {
  std::cout << "[xdg description] " << description << std::endl;
};

const struct zxdg_output_v1_listener xdg_output_listener = {
    .logical_position = xdg_output_logical_position,
    .logical_size = xdg_output_logical_size,
    .done = xdg_output_done,
    .name = xdg_output_name,
    .description = xdg_output_description,
};
