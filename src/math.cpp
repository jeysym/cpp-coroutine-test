#include "math.hpp"
#include <cmath>

float Vec2::length() const { return std::sqrt(x * x + y * y); }

bool Vec2::isZero() const { return length() < 0.001f; }

void Vec2::normalize() {
  if (length() > 0.001f) {
    const float len = length();
    x /= len;
    y /= len;
  }
}

Vec2 &Vec2::operator+=(const Vec2 &other) {
  x += other.x;
  y += other.y;
  return *this;
}

Vec2 operator*(float k, const Vec2 &v) {
  return Vec2{.x = k * v.x, .y = k * v.y};
}

Vec2 operator+(const Vec2 &a, const Vec2 &b) {
  return Vec2{.x = a.x + b.x, .y = a.y + b.y};
}
