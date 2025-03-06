#include <ft2build.h>
#include <hb-ft.h>
#include <hb.h>

#include FT_FREETYPE_H

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

  float x = 0.0f, y = 100.0f;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);

  glColor3f(1.0f, 1.0f, 1.0f);

  for (unsigned int i = 0; i < glyph_count; i++) {
    hb_codepoint_t glyph_id = glyph_info[i].codepoint;

    if (FT_Load_Glyph(face, glyph_id, FT_LOAD_RENDER)) {
      continue;
    }

    GLuint texture = create_texture_from_glyph(face->glyph->bitmap);

    float w = face->glyph->bitmap.width;
    float h = face->glyph->bitmap.rows;
    float x_offset = face->glyph->bitmap_left;
    float y_offset = face->glyph->bitmap_top;

    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(x + x_offset, y - y_offset);
    glTexCoord2f(1, 0);
    glVertex2f(x + x_offset + w, y - y_offset);
    glTexCoord2f(1, 1);
    glVertex2f(x + x_offset + w, y - y_offset - h);
    glTexCoord2f(0, 1);
    glVertex2f(x + x_offset, y - y_offset - h);
    glEnd();

    glDeleteTextures(1, &texture);

    x += glyph_pos[i].x_advance / 64.0;
    y += glyph_pos[i].y_advance / 64.0;
  }

  hb_buffer_destroy(buf);
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Text Rendering", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    return -1;
  }

  glfwMakeContextCurrent(window);

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
    glViewport(0, 0, 800, 600);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    render_text("Hello", face, hb_font);

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