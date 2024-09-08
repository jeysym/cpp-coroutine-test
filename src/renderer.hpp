#include <GL/gl.h>
#include <optional>

namespace Renderer {

struct Texture {
  int m_Width = 0;
  int m_Height = 0;
  int m_NumComponents = 0;
  unsigned char *m_Data = nullptr;
  GLuint m_GLTexId = 0;

  /// Loads the texture data from file. This is a blocking call.
  static std::optional<Texture> loadFromFile(const char *filePath);
};

void init();
void render(float deltaSeconds);

} // namespace Renderer
