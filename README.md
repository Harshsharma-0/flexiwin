## flexiwin
- flexiwin is a wayland specific implementaion of methods that allows easy creation of windows on wayland platfroms and to manage them,
  all methods of flexiwin is simple to use and can be integrated into a application easily, either to create a window or to create a custom window manager for specific use case, and more.

> [!NOTE]
  Must check platform compatablity beform using flexiwin.


# Usage
 - Clone the repo and build.
 ```bash
    git clone https://Harsharma-0/flexiwin.git
    cd flexiwin && mkdir build
    cmake .. && make
 ```
- In a bare project, statically link against ```libflexiwin.a``` located in the build folder, and add include path to the flexiwin include folder.
- In a cmake project add ```add_subdirectory(path to the flexiwin directory)```.


# A minimum flexiwin opengl example

```cpp
#include "flexiwin/flexiwin.hpp"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <iostream>

flexiwin_state wmstate = {0};
flexiwin_egl_info eglstate = {0};

const char *appname = "flexiwin opengl example";

EGLint fb_attr[] = {EGL_SURFACE_TYPE,
                    EGL_WINDOW_BIT,
                    EGL_RENDERABLE_TYPE,
                    EGL_OPENGL_ES2_BIT,
                    EGL_RED_SIZE,
                    8,
                    EGL_GREEN_SIZE,
                    8,
                    EGL_BLUE_SIZE,
                    8,
                    EGL_NONE};
EGLint ctx_attr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE};

auto exited = [](void *data) -> void {
  std::cout << "[info] window exited" << std::endl;
};
auto key = [](uint32_t event_type, uint32_t time, uint32_t key, uint32_t serial,
              void *data) {
  std::cout << "[info] key event " << (char)key << std::endl;
};
auto mouse = [](struct flexiwin_pointer_event *event, void *data) {
  std::cout << "[info] mouse event" << std::endl;
};

int main() {

  eglstate.ctx_attribs = ctx_attr;
  eglstate.config_attribs = fb_attr;
  eglstate.num_cfg = 1;

  //initlises the flexiwin instance
  flexiwin_init(&wmstate, appname, 300, 300, window_type_dynamic);

  flexiwin_enable_gl(&wmstate, &eglstate); // enable opengl in flexiwin
  flexiwin_create(&wmstate); // create flexiwin, created window
  flexiwin_create_gl_ctx(&wmstate); // create the context and bind the surface

  /*
   * **NOTE** must configure callback of key as not doing that causes seg fault.
   * Callback to capture the raw keys event's and and store in implementation
   * independent manner
   *
   */
  flexiwin_setup_key_callback(&wmstate, key, &wmstate);

  /* **NOTE** must configure callback of pointer as not doing that causes seg fault.
   * Callback used to extract the mouse event, and the implementation can
   * translate that into something usefull for their purpose
   */
  flexiwin_setup_pointer_callback(&wmstate, mouse, &wmstate);

  /* **NOTE** must setup loop callback before calling the evloop
   * function to bind the function to run inside the loop
   * the input data is the ptr to data that you passed
   */
  flexiwin_setup_loop(
      &wmstate,
      [](void *data) -> bool {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        eglSwapBuffers(eglstate.display, wmstate.surface);
        wl_display_dispatch(wmstate.display);
        /*
         * return true to exit loop;
         * or call flexiwin_close_window(state);
         */
        return false;
      },
      nullptr);

  /*
   * function to setup exit callback to run after the main evloop exits
   */
  flexiwin_setup_exit_callback(&wmstate, exited);

  /*
   * function to run the evloop
   */
  flexiwin_run_loop(&wmstate);
  /*
   * use these function to destroy instance properly if using own manual loop
   * and custom loop;
   * flexiwin_destroy_egl(&eglstate);
   * flexiwin_destroy(&wmstate);
   */
  return 0;
}
```

> [!NOTE]
  Refer to example directory for more example and usages.
