#include "renderer.hpp"
#include "game.hpp"
#include "stb/stb_image.h"
#include <GL/glu.h>
#include <iostream>

namespace Renderer {

std::optional<Texture> Texture::loadFromFile(const char *filePath) {
  Texture result;

  result.m_Data = stbi_load(filePath, &result.m_Width, &result.m_Height,
                            &result.m_NumComponents, 0);

  if (result.m_Data) {
    glGenTextures(1, &result.m_GLTexId);
    glBindTexture(GL_TEXTURE_2D, result.m_GLTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.m_Width, result.m_Height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, result.m_Data);
    return result;
  } else {
    std::cerr << "[Texture] Could not load texture from file: " << filePath
              << std::endl;
    return std::nullopt;
  }
}

Texture characterTexture;

void init() {
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_TEXTURE_2D);
  characterTexture = Texture::loadFromFile("character.png").value();
}

void render(float deltaSeconds) {
  float fadeRatio = Game::g_State.m_FadeRatio;
  Vec2 playerPos = Game::g_State.m_PlayerPosition;

  glClearColor(0.0f, 0.3f, 0.0f, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TODO: Do not use immediate mode OpenGL.

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1., 1., -1., 1., 1., 20.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D, characterTexture.m_GLTexId);

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

  for (auto &bullet : Game::g_State.m_Bullets) {
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-.01 + bullet.m_Position.x, -.01 + bullet.m_Position.y, 0.);
    glVertex3f(.01 + bullet.m_Position.x, -.01 + bullet.m_Position.y, 0.);
    glVertex3f(.01 + bullet.m_Position.x, .01 + bullet.m_Position.y, 0.);
    glVertex3f(-.01 + bullet.m_Position.x, .01 + bullet.m_Position.y, 0.);
    glEnd();
  }

  glDisable(GL_TEXTURE_2D);

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

} // namespace Renderer
