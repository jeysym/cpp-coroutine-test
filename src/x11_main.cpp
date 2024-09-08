#include <X11/X.h>
#include <X11/Xlib.h>

#include "renderer.hpp"
#include <GL/glx.h>

#include <ctime>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <unistd.h>
#include <map>

#include "timers.hpp"
#include "co_task.hpp"
#include "game.hpp"
#include "input.hpp"

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

std::map<KeyCode, Input::Action> g_ActionMap;

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
  g_SWA.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;

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

  g_ActionMap[XKeysymToKeycode(g_Display, XK_w)] = Input::Action::UP;
  g_ActionMap[XKeysymToKeycode(g_Display, XK_s)] = Input::Action::DOWN;
  g_ActionMap[XKeysymToKeycode(g_Display, XK_a)] = Input::Action::LEFT;
  g_ActionMap[XKeysymToKeycode(g_Display, XK_d)] = Input::Action::RIGHT;
  g_ActionMap[XKeysymToKeycode(g_Display, XK_Escape)] = Input::Action::QUIT;
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
        auto it = g_ActionMap.find(event.xkey.keycode);
        if (it != g_ActionMap.end()) {
          Input::setActionActive(it->second, true);
        }
      }
      else if (event.type == KeyRelease) {
        auto it = g_ActionMap.find(event.xkey.keycode);
        if (it != g_ActionMap.end()) {
          Input::setActionActive(it->second, false);
        }
      }
    }

    if (Input::getActionActive(Input::Action::QUIT)) {
      glXMakeCurrent(g_Display, None, nullptr);
      glXDestroyContext(g_Display, g_GLContext);
      XDestroyWindow(g_Display, g_Window);
      XCloseDisplay(g_Display);
      std::exit(0);
    }        
    
    Game::update(elapsedSeconds);
    Renderer::render(elapsedSeconds);
    glXSwapBuffers(g_Display, g_Window);
  }
}
