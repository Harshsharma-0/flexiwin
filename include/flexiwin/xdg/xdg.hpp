#ifndef __FLEXON_XDG_PIPE__
#define __FLEXON_XDG_PIPE__
#include <wayland-client.h>


extern const struct xdg_wm_base_listener xdg_wm_base_callback_listener; 
extern const struct xdg_surface_listener xdg_surface_callback_listener;
extern const struct xdg_toplevel_listener xdg_surface_callback_listener_toplevel;
extern const struct zxdg_output_v1_listener xdg_output_listener; 

 
#endif



