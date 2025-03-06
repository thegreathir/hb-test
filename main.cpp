#include <hb.h>
#include <iostream>

// Simple function that prints HarfBuzz version
void print_hb_version() {
    std::cout << "HarfBuzz version: " << hb_version_string() << std::endl;
}

// Function that creates a HarfBuzz font from a file (dummy usage)
void create_hb_font(const char* font_path) {
    hb_blob_t* blob = hb_blob_create_from_file(font_path);
    if (blob) {
        hb_face_t* face = hb_face_create(blob, 0);  // Create a face from the blob
        if (face) {
            hb_font_t* font = hb_font_create(face);  // Create the font from the face
            hb_face_destroy(face);  // Clean up the face
            if (font) {
                std::cout << "Font created successfully!" << std::endl;
                hb_font_destroy(font);
            } else {
                std::cerr << "Failed to create font!" << std::endl;
            }
        } else {
            std::cerr << "Failed to create face!" << std::endl;
        }
        hb_blob_destroy(blob);  // Clean up the blob
    } else {
        std::cerr << "Failed to load font!" << std::endl;
    }
}

int main() {
    print_hb_version();
    create_hb_font("/path/to/font.ttf");  // Replace with a valid font path if needed
    return 0;
}
