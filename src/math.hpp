
struct Vec2 {
  float x = 0.0f;
  float y = 0.0f;

  float length() const;
  void normalize();

  Vec2& operator+=(const Vec2& other);
};

Vec2 operator*(float k, const Vec2& v);
Vec2 operator+(const Vec2& a, const Vec2& b);
