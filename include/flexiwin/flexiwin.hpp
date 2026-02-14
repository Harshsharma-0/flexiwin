#pragma once
#pragma once

#include "./common.hpp"
#include "./wayland-callback.hpp"
#include "./xdg-output-client-protocol.h"
#include "./xdg-shell-client-protocol.h"
#include "flexiwin/wayland-callback.hpp"
#include "flexiwin/xdg/xdg.hpp"
#include <cstdint>

int flexiwin_init(flexiwin_state *state, char *appname, int width, int height,
                  window_mode mode, window_type type);
int flexiwin_moveXY(int x, int y);
int flexiwin_resize(int width, int height);
int flexiwin_get_win_fd();
int flexiwin_destroy(flexiwin_state *state, int retVal);
