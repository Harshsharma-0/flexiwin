#include "flexiwin/flexiwin.hpp"
#include <iostream>
flexiwin_state wmstate = {0};
int main() {
  flexiwin_init(&wmstate, "fgdf", 300, 300, window_mode::customSize,
                window_type::win_dynamic);
  flexiwin_destroy(&wmstate, 0);
  return 0;
}
