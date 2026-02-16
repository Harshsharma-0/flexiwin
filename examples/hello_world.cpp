#include "flexiwin/common.hpp"
#include "flexiwin/flexiwin.hpp"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cstring>
#include <iostream>
#include <wayland-client-core.h>
flexiwin_state wmstate = {0};
const char *appname = "iuf";

int main() {

  flexiwin_init(&wmstate, (char *)appname, 300, 300, window_type::win_dynamic);
  flexiwin_create(&wmstate, window_mode::customSize);
  memset(wmstate.rawPixels, 0xffffffff, wmstate.local_info.size);
  wl_surface_damage(wmstate.surface, 0, 0, wmstate.local_info.width,
                    wmstate.local_info.height);
  wl_surface_commit(wmstate.surface);

  while (wmstate.mask & FLEXI_WINDOW_RUNNING) {
    wl_display_dispatch(wmstate.display);
    memset(wmstate.rawPixels, 0xffffffff, wmstate.local_info.size);
    wl_surface_damage(wmstate.surface, 0, 0, wmstate.local_info.width,
                      wmstate.local_info.height);
    wl_surface_commit(wmstate.surface);
  };
  flexiwin_destroy(&wmstate);
  return 0;
}
