#include <ft2build.h>
#include FT_FREETYPE_H
#include <hb-ft.h>
#include <hb.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

GLuint create_texture_from_glyph(const FT_Bitmap &bitmap) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, bitmap.width, bitmap.rows, 0,
               GL_ALPHA, GL_UNSIGNED_BYTE, bitmap.buffer);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return texture;
}

void render_text(const std::string &text, FT_Face face, hb_font_t *hb_font) {
  hb_buffer_t *buf = hb_buffer_create();
  hb_buffer_add_utf8(buf, text.c_str(), text.size(), 0, -1);
  hb_buffer_guess_segment_properties(buf);
  hb_shape(hb_font, buf, nullptr, 0);

  unsigned int glyph_count;
  hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
  hb_glyph_position_t *glyph_pos =
      hb_buffer_get_glyph_positions(buf, &glyph_count);

  float pen_x = 50.0f, pen_y = 360.0f;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);

  for (unsigned int i = 0; i < glyph_count; i++) {
    hb_codepoint_t glyph_id = glyph_info[i].codepoint;
    if (FT_Load_Glyph(face, glyph_id, FT_LOAD_RENDER)) {
      continue;
    }

    FT_GlyphSlot glyph = face->glyph;
    GLuint texture = create_texture_from_glyph(glyph->bitmap);

    float w = static_cast<float>(glyph->bitmap.width);
    float h = static_cast<float>(glyph->bitmap.rows);
    float xpos = pen_x + glyph->bitmap_left + glyph_pos[i].x_offset / 64.0f;
    float ypos = pen_y - glyph->bitmap_top - glyph_pos[i].y_offset / 64.0f;

    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(xpos, ypos);
    glTexCoord2f(1, 0);
    glVertex2f(xpos + w, ypos);
    glTexCoord2f(1, 1);
    glVertex2f(xpos + w, ypos + h);
    glTexCoord2f(0, 1);
    glVertex2f(xpos, ypos + h);
    glEnd();

    glDeleteTextures(1, &texture);

    pen_x += glyph_pos[i].x_advance / 64.0f;
    pen_y += glyph_pos[i].y_advance / 64.0f;
  }
  hb_buffer_destroy(buf);
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }

  GLFWwindow *window =
      glfwCreateWindow(1280, 720, "Text Rendering", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW\n";
    return -1;
  }

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cerr << "Failed to initialize FreeType\n";
    return -1;
  }

  FT_Face face;
  if (FT_New_Face(ft, "DejaVuSans.ttf", 0, &face)) {
    std::cerr << "Failed to load font\n";
    return -1;
  }
  FT_Set_Pixel_Sizes(face, 0, 48);

  hb_font_t *hb_font = hb_ft_font_create(face, nullptr);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const auto text = "به نام خداوند رنگین کمان";
    render_text(text, face, hb_font);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  hb_font_destroy(hb_font);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
