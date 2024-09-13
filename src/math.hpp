#pragma once

struct Vec2 {
  float length() const;
  bool isZero() const;
  void normalize();
  Vec2 &operator+=(const Vec2 &other);

  float x = 0.0f;
  float y = 0.0f;
};

Vec2 operator*(float k, const Vec2 &v);
Vec2 operator+(const Vec2 &a, const Vec2 &b);
