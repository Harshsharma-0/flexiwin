#pragma once

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <wayland-egl-core.h>

#include "./common.hpp"
#include "./wayland-callback.hpp"
#include "./xdg-output-client-protocol.h"
#include "./xdg-shell-client-protocol.h"
#include "./xdg/xdg.hpp"
#include "flexiwin/events/common.hpp"
#include "flexiwin/events/keyboard.hpp"
#include "flexiwin/events/mouse.hpp"
#include <GLES2/gl2.h>
#include <cstdint>

// structure to support opengl
typedef struct flexiwin_egl_info {
  wl_egl_window *window;
  wl_surface *surface;
  EGLint *ctx_attribs;
  EGLint *config_attribs;

  EGLDisplay display;
  EGLint major_ver;
  EGLint minor_ver;
  EGLContext context;
  EGLConfig config;
  EGLint num_cfg;
} flexiwin_egl_info;

// flexiwin_box used to return height and width
typedef struct {
  int32_t height;
  int32_t width;
} flexiwin_box;

/**
 * @brief initlises the instance and setup some internal parameters
 *
 * @param state[in] pointer to the state object
 * @param appname[in] pointer to the appname
 * @param width[in] width of the window to create
 * @param height[in] height of the window to create
 *
 * @return 0 on Success and negative value to indicate error;
 */
int flexiwin_init(flexiwin_state *state, const char *appname, int width,
                  int height, int window_type);

/**
 * @brief create the window
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return 0 on Success, negative value to indicate error;
 */

int flexiwin_create(flexiwin_state *state);

/**
 * @brief enable opengl on the flexiwin instance
 * @param[in] state pointer to the flexiwin state
 * @param[in] egl_info pointer to the flexiwin egl state
 *
 * @return void
 */
inline void flexiwin_enable_gl(flexiwin_state *state,
                               flexiwin_egl_info *egl_info) {
  egl_info->window = NULL;
  egl_info->context = EGL_NO_CONTEXT;
  egl_info->display = EGL_NO_DISPLAY;
  egl_info->surface = (wl_surface *)EGL_NO_SURFACE;
  state->egl_info = egl_info;
};

/**
 * @brief create opengl context for flexiwin
 *
 * @param[in] state pointer to the instance of flexiwin
 *
 * @return 0 on Success, negative value to indicate error
 * @note Must enable opengl using ```flexiwin_enable_gl```, otherwise calling
 * function will yields error.
 */
int flexiwin_create_gl_ctx(flexiwin_state *state);

/**
 * @brief destroy opengl instance created
 * @param[in] egl_info pointer to the instance of egl_info;
 *
 * @return void
 */
void flexiwin_destroy_egl(flexiwin_egl_info *egl_info);

/**
 * @brief destroy flexiwin instance
 * @param [in] state pointer to the flexiwin state
 * @return void
 */
void flexiwin_destroy(flexiwin_state *state);

/**
 * @brief returns the display fd of the window
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return displayFd
 */
inline int flexiwin_get_win_fd(flexiwinState *state) {
  return state->displayFd;
};

/**
 * @brief setup the callback for the main event loop
 *
 *  ```typesignature of the callback void(*)(void *data)```
 *
 * @param [in] state pointer to the instance of flexiwin
 * @param [in] callback pointer to the fucntion, used as callback
 * @param [in] data pointer to the data to pass to the callback
 * @return void
 *
 *
 */

inline void flexiwin_setup_loop(flexiwin_state *state,
                                flexiwin_while_running callback, void *data) {
  state->whileRunning = callback;
  state->data = data;
};

/**
 * @brief setup the callback for the key events
 *
 * ```typesignature of the callback void (*)(uint32_t event_type, uint32_t time,
 *                                      uint32_t key, uint32_t serial,
 *                                    void *data);
 *  ```
 *  callback[param] event_type : refer to include/flexiwin/events/keyboard.hpp enum of keyboard event type
 *  callback[param] time : timestamp of the keypress
 *  callback[param] serial : wayland input serial
 *  callback[param] data : pointer the the data passed
 *
 * @param [in] state pointer to the instance of flexiwin
 * @param [in] callback pointer to the fucntion, used as callback
 * @param [in] data pointer to the data to pass to the callback
 * @return void
 *
 * @note used this only to collect data, not to process, process data in main
event loop
 */

inline void flexiwin_setup_key_callback(flexiwin_state *state,
                                        flexiwin_key_callback callback,
                                        void *data) {
  state->keyEvent.callback = callback;
  state->keyEvent.data = data;
};

/**
 * @brief setup the callback for the pointer events
 *
 * ```typesignature of the callback void (*)(flexiwin_pointer_event *event,void *data);
 *  ```
 *  callback[param] event : pointer to structure ```flexiwin_pointer_event```, refer to include/flexiwin/events/mouse.hpp
 *  callback[param] data : pointer to the data passed
 *
 * @param [in] state pointer to the instance of flexiwin
 * @param [in] callback pointer to the fucntion, used as callback
 * @param [in] data pointer to the data to pass to the callback
 * @return void
 *
 * @note used this only to collect data, not to process, process data in main event loop
*/

inline void flexiwin_setup_pointer_callback(flexiwin_state *state,
                                            flexiwin_pointer_frame_cb callback,
                                            void *data) {
  state->pointerEvent.pointer_cb = callback;
  state->pointerEvent.data = data;
};

/**
 * @brief setup callback to call after the evloop exit
 *   tyesignature of callback: ```void (*)(void *data)```
 *
 *  @param[in] state pointer to the instance of flexiwin
 *  @param[in] callback pointer to the function, used as callback
 * 
 *  @return void
 */

inline void flexiwin_setup_exit_callback(flexiwin_state *state,
                                         flexiwin_on_exit callback) {
  state->onExit = callback;
};

/**
 * @brief check's if the window is currently maximized
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return true if condition is met, false it not
 *
 */

inline bool flexiwin_is_maximized(flexiwin_state *state) {
  return (state->mask & FLEXI_WIN_MAXIMIZED);
};

/**
 * @brief check's if the window is currently minimized
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return true if condition is met, false it not
 *
 */

inline bool flexiwin_is_minimized(flexiwin_state *state) {
  return (state->mask & FLEXI_WIN_MINIMIZED);
};

/**
 * @brief check's if the window is resized
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return true if condition is met, false it not
 *
 */

inline bool flexiwin_window_resized(flexiwin_state *state) {
  bool resized = (state->mask & FLEXI_WINDOW_RESIZED);
  state->mask &= FLEXI_WINDOW_RESIZED;
  return resized;
};

/**
 * @brief check's if the window is running
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return true if condition is met, false it not
 *
 */

inline bool flexiwin_is_running(flexiwin_state *state) {
  return (state->mask & FLEXI_WINDOW_RUNNING);
};

/**
 * @brief function of get the resize_type
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return resize_type, refer to include/flexiwin/events/common.hpp
 *
 */

inline int flexiwin_get_resize_type(flexiwin_state *state) {
  return state->resize_type;
};

/**
 * @brief function of get the dimension of window in pixels
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return flexiwin_box
 *
 */

inline flexiwin_box flexiwin_get_window_box(flexiwin_state *state) {
  return {.height = state->local_info.height, .width = state->local_info.width};
};

/**
 * @brief function of get the dimension of monitor in pixels
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return flexiwin_box
 *
 */

inline flexiwin_box flexiwin_get_monitor_size(flexiwin_state *state) {
  return {.height = state->display_info.height,
          .width = state->display_info.width};
};

/**
 * @brief helper function to close the window
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return void
 *
 */

inline void flexiwin_close_window(flexiwin_state *state) {
  state->mask &= FLEXI_WINDOW_RUNNING;
}

/**
 * @brief main eventloop of the flexiwin
 *
 * @param[in] state pointer to the instance of flexiwin
 * @return void
 *
 */
inline void flexiwin_run_loop(flexiwin_state *state) {
  bool exit = false;
  while (state->mask & FLEXI_WINDOW_RUNNING && exit == false) {
    exit = state->whileRunning(state->data);
  };
  state->onExit(state->data);
  flexiwin_destroy_egl(state->egl_info);
  flexiwin_destroy(state);
};

#define FLEXIWIN_SET_CHECK(typechk, type, rval)                                \
  if (typechk(state->mask & type) && state->xdg_surface_toplevel != NULL &&    \
      state->win_type != window_type_static)                                   \
  rval

/**
 * @brief function to make the window maximized
 * @param[in] pointer to the instance of flexiwin
 * @return 0 on Success, negative value indicated error
 */

inline int flexiwin_set_maximized(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(!, FLEXI_WIN_MAXIMIZED, {
    xdg_toplevel_set_maximized(state->xdg_surface_toplevel);
    state->mask |= FLEXI_WIN_MAXIMIZED;
    return 0;
  })
  return -1;
};

/**
 * @brief function to make the window manimized
 * @param[in] pointer to the instance of flexiwin
 * @return 0 on Success, negative value indicated error
 */

inline int flexiwin_set_minimized(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(!, FLEXI_WIN_MINIMIZED, {
    xdg_toplevel_set_minimized(state->xdg_surface_toplevel);
    state->mask |= FLEXI_WIN_MINIMIZED;
    return 0;
  })
  return -1;
};

/**
 * @brief function to make the window full screen
 * @param[in] pointer to the instance of flexiwin
 * @return 0 on Success, negative value indicated error
 */

inline int flexiwin_set_fullscreen(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(!, FLEXI_WIN_FULLSCREEN, {
    xdg_toplevel_set_fullscreen(state->xdg_surface_toplevel,
                                state->display_output);
    state->mask |= FLEXI_WIN_FULLSCREEN;
    return 0;
  })
  return -1;
};

/**
 * @brief function to unset full screen
 * @param[in] pointer to the instance of flexiwin
 * @return 0 on Success, negative value indicated error
 */

inline int flexiwin_unset_fullscreen(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(, FLEXI_WIN_FULLSCREEN, {
    xdg_toplevel_unset_fullscreen(state->xdg_surface_toplevel);
    state->mask &= ~FLEXI_WIN_FULLSCREEN;
    return 0;
  })
  return -1;
}

/**
 * @brief function to unset maximized
 * @param[in] pointer to the instance of flexiwin
 * @return 0 on Success, negative value indicated error
 */

inline int flexiwin_unset_maximized(flexiwin_state *state) {
  FLEXIWIN_SET_CHECK(, FLEXI_WIN_MAXIMIZED, {
    xdg_toplevel_unset_maximized(state->xdg_surface_toplevel);
    state->mask &= ~FLEXI_WIN_MAXIMIZED;
    return 0;
  })
  return -1;
};
#undef FLEXIWIN_SET_CHECK


/**
 * @brief function to start interactive session to mode window
 * @param[in] pointer to the instance of flexiwin
 * @return void  
 */

inline void flexiwin_moveXY(flexiwin_state *state, uint32_t serial) {
  if (state->xdg_surface_toplevel != NULL)
    xdg_toplevel_move(state->xdg_surface_toplevel, state->display_seat, serial);
};

/**
 * Here parameter edge is the xdg-egde enum for the specific edge to resize from
 * Example : flexiwin_resize(&state,serial,XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM);
 *    To resize from the bottom edge
 */

/**
 * @brief function to start interactive resize
 * @param[in] pointer to the instance of flexiwin
 * @param[in] serial serial of event that results in this action
 * @param[in] edge edge enum from which to resize the window
 * @return void  
 */

inline void flexiwin_resize(flexiwin_state *state, uint32_t serial,
                            uint32_t edge) {
  if (state->xdg_surface_toplevel != NULL)
    xdg_toplevel_resize(state->xdg_surface_toplevel, state->display_seat,
                        serial, edge);
};


/**
 * @brief function to show options menu
 * @param[in] pointer to the instance of flexiwin
 * @return void  
 */

inline void flexiwin_show_menu(flexiwin_state *state, uint32_t serial, int x,
                               int y) {
  if (state->xdg_surface_toplevel != NULL)
    xdg_toplevel_show_window_menu(state->xdg_surface_toplevel,
                                  state->display_seat, serial, x, y);
};
