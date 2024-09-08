#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
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

static int img_width = 0;
static int img_height = 0;
static int img_dim = 0;
static char* img_data = nullptr;
static GLuint img_texture = 0;

void gl_render_test_quad() {
  float fadeRatio = g_State.m_FadeRatio;
  Vec2 playerPos = g_State.m_PlayerPosition;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_BLEND);

  // TODO: Do not use immediate mode OpenGL.

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1., 1., -1., 1., 1., 20.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, img_texture);

  glBegin(GL_QUADS);

  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-.1 + playerPos.x, -.1 + playerPos.y, 0.);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(.1 + playerPos.x, -.1 + playerPos.y, 0.);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(.1 + playerPos.x, .1 + playerPos.y, 0.);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-.1 + playerPos.x, .1 + playerPos.y, 0.);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBegin(GL_QUADS);
  glColor4f(0., 0., 0., fadeRatio);
  glVertex3f(-1.0, -1.0, 1.);
  glColor4f(0., 0., 0., fadeRatio);
  glVertex3f(1.0, -1.0, 1.);
  glColor4f(0., 0., 0., fadeRatio);
  glVertex3f(1.0, 1.0, 1.);
  glColor4f(0., 0., 0., fadeRatio);
  glVertex3f(-1.0, 1.0, 1.);
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

  KC_W = XKeysymToKeycode(g_Display, XK_w);
  KC_A = XKeysymToKeycode(g_Display, XK_a);
  KC_S = XKeysymToKeycode(g_Display, XK_s);
  KC_D = XKeysymToKeycode(g_Display, XK_d);
  KC_ESC = XKeysymToKeycode(g_Display, XK_Escape);
}

void gl_init() {
  glEnable(GL_DEPTH_TEST);

  img_data = (char*)stbi_load("character.png", &img_width, &img_height, &img_dim, 0);

  if (img_data) {
    glGenTextures(1, &img_texture);
    glBindTexture(GL_TEXTURE_2D, img_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
  }
  else {
    std::cerr << "Could not load the image" << std::endl;
  }
}

int main(int arg_count, char** args) {
  x11_init("JEY WINDOW");
  gl_init();
  Game::init();
  
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
    gl_render_test_quad();
    glXSwapBuffers(g_Display, g_Window);
  }
}
