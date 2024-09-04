#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <iostream>
#include <cstdlib>
#include <time.h>
#include <unistd.h>

#include "timers.hpp"
#include "co_task.hpp"

static Display *g_Display = nullptr;
static Window g_RootWindow;
static GLint g_AttribList[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
static XVisualInfo *g_Visual = nullptr;
static Colormap g_Colormap;
static XSetWindowAttributes g_SWA;
static Window g_Window;
static GLXContext g_GLContext;
static XWindowAttributes g_WindowAttributes;

void gl_render_test_quad() {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1., 1., -1., 1., 1., 20.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

  glBegin(GL_QUADS);
  glColor3f(1., 0., 0.);
  glVertex3f(-.75, -.75, 0.);
  glColor3f(0., 1., 0.);
  glVertex3f(.75, -.75, 0.);
  glColor3f(0., 0., 1.);
  glVertex3f(.75, .75, 0.);
  glColor3f(1., 1., 0.);
  glVertex3f(-.75, .75, 0.);
  glEnd();
}

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
}

void gl_init() {
  glEnable(GL_DEPTH_TEST);
}

int main(int arg_count, char** args) {
  x11_init("JEY WINDOW");
  gl_init();
  
  while (true) {
    XEvent event;
    XNextEvent(g_Display, &event);

    if (event.type == Expose) {
      XGetWindowAttributes(g_Display, g_Window, &g_WindowAttributes);
      glViewport(0, 0, g_WindowAttributes.width, g_WindowAttributes.height);
      gl_render_test_quad();
      glXSwapBuffers(g_Display, g_Window);
    }
    else if (event.type == KeyPress) {
      glXMakeCurrent(g_Display, None, nullptr);
      glXDestroyContext(g_Display, g_GLContext);
      XDestroyWindow(g_Display, g_Window);
      XCloseDisplay(g_Display);
      std::exit(0);
    }
  }
}
