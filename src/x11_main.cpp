#include <X11/X.h>
#include <X11/Xlib.h>

#include "renderer.hpp"
#include <GL/glx.h>

#include <ctime>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <unistd.h>

#include "timers.hpp"
#include "co_task.hpp"
#include "game.hpp"

#include "stb/stb_image.h"

static Display *g_Display = nullptr;
static Window g_RootWindow;
static GLint g_AttribList[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
static XVisualInfo *g_Visual = nullptr;
static Colormap g_Colormap;
static XSetWindowAttributes g_SWA;
static Window g_Window;
static GLXContext g_GLContext;
static XWindowAttributes g_WindowAttributes;

static KeyCode KC_W = 0;
static KeyCode KC_A = 0;
static KeyCode KC_S = 0;
static KeyCode KC_D = 0;
static KeyCode KC_ESC = 0;

void x11_init(const char* window_name) {
  g_Display = XOpenDisplay(nullptr);

  if (g_Display == nullptr) {
    std::cerr << "[X11] Cannot connect to X server." << std::endl;
    std::exit(0);
  }

  g_RootWindow = DefaultRootWindow(g_Display);

  if (!g_RootWindow) {
    std::cerr << "[X11] Could not get the root window." << std::endl;
    std::exit(0);
  }
  
  g_Visual = glXChooseVisual(g_Display, 0, g_AttribList);

  if (g_Visual == nullptr) {
    std::cerr << "[X11] No apropriate visual found." << std::endl;
    std::exit(0);
  } 

  g_Colormap = XCreateColormap(g_Display, g_RootWindow, g_Visual->visual, AllocNone);

  g_SWA.colormap = g_Colormap;
  g_SWA.event_mask = ExposureMask | KeyPressMask;

  g_Window = XCreateWindow(g_Display, g_RootWindow, 0, 0, 600, 600, 0, g_Visual->depth, InputOutput,
                      g_Visual->visual, CWColormap | CWEventMask, &g_SWA);

  if (!g_Window) {
    std::cerr << "[X11] Window could not be created." << std::endl;
    std::exit(0);
  }

  XMapWindow(g_Display, g_Window);
  XStoreName(g_Display, g_Window, window_name);

  g_GLContext = glXCreateContext(g_Display, g_Visual, nullptr, GL_TRUE);

  if (!g_GLContext) {
    std::cerr << "[X11] OpenGL context could not be created." << std::endl;
    std::exit(0);
  }
  
  glXMakeCurrent(g_Display, g_Window, g_GLContext);

  KC_W = XKeysymToKeycode(g_Display, XK_w);
  KC_A = XKeysymToKeycode(g_Display, XK_a);
  KC_S = XKeysymToKeycode(g_Display, XK_s);
  KC_D = XKeysymToKeycode(g_Display, XK_d);
  KC_ESC = XKeysymToKeycode(g_Display, XK_Escape);
}

int main(int arg_count, char** args) {
  x11_init("JEY WINDOW");
  Game::init();
  Renderer::init();
  
  timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now)) {
    std::cerr << "Call to 'clock_gettime' has failed." << std::endl;
    std::exit(0);
  }

  while (true) {
    timespec prev = now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) {
      std::cerr << "Call to 'clock_gettime' has failed." << std::endl;
      std::exit(0);
    }

    const double secondsPart = (double)(now.tv_sec - prev.tv_sec);
    const double nanosecondPart = (double)(now.tv_nsec - prev.tv_nsec) / 1'000'000'000.f;
    const double elapsedSeconds = secondsPart + nanosecondPart;

    while (XPending(g_Display) > 0) {
      XEvent event;
      XNextEvent(g_Display, &event);
      
      if (event.type == Expose) {
        XGetWindowAttributes(g_Display, g_Window, &g_WindowAttributes);
        glViewport(0, 0, g_WindowAttributes.width, g_WindowAttributes.height);
      }
      else if (event.type == KeyPress) {
        KeyCode kc = event.xkey.keycode;

        if (kc == KC_W) {
            g_State.m_PlayerPosition.y += elapsedSeconds;
        }
        if (kc == KC_S) {
            g_State.m_PlayerPosition.y -= elapsedSeconds;
        }
        if (kc == KC_A) {
            g_State.m_PlayerPosition.x -= elapsedSeconds;
        }
        if (kc == KC_D) {
            g_State.m_PlayerPosition.x += elapsedSeconds;
        }        
        if (kc == KC_ESC) {
          glXMakeCurrent(g_Display, None, nullptr);
          glXDestroyContext(g_Display, g_GLContext);
          XDestroyWindow(g_Display, g_Window);
          XCloseDisplay(g_Display);
          std::exit(0);
        }        
        
      }
    }

    Game::update(elapsedSeconds);
    Renderer::render(elapsedSeconds);
    glXSwapBuffers(g_Display, g_Window);
  }
}
