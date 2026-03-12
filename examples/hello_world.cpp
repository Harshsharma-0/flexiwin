#include "flexiwin/flexiwin.hpp"
#include <cstring>

flexiwin_state wmstate = {0};
const char *appname = "flexiwin example";

int main() {

  flexiwin_init(&wmstate, appname, 300, 300, window_type::win_dynamic);
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
