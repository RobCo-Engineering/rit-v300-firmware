#include "fonts.h"

#include "fonts/TRI88CSO.c"
#include "fonts/TRI89B.c"
#include "fonts/apri256.c"
#include "fonts/tipc9.c"
#include "fonts/vga8x16.c"
#include "fonts/z100.c"

// Instantiate the font struct
const BitmapFont vga_font_8x16 = {
    .glyphs = VGA8_F16, .char_width = 8, .char_height = 16, .bytes_per_row = 1, .first_char = 0, .last_char = 255};

const BitmapFont apri_font_8x10 = {
    .glyphs = APRI256_F10, .char_width = 8, .char_height = 10, .bytes_per_row = 1, .first_char = 0, .last_char = 255};

const BitmapFont z100_font = {
    .glyphs = Z100_A_F09, .char_width = 8, .char_height = 9, .bytes_per_row = 1, .first_char = 0, .last_char = 255};

// TI Professional Computer (adapted for VGA), 9x12
const BitmapFont tipc_font_9x12 = {
    .glyphs = TI_PC_9_F12, .char_width = 9, .char_height = 12, .bytes_per_row = 1, .first_char = 0, .last_char = 255};

const BitmapFont TRI88CSO_font_8x11 = {.glyphs        = ___vga_text_mode_fonts_FONTS_PC_OTHER_TRI88CSO_F11,
                                       .char_width    = 8,
                                       .char_height   = 11,
                                       .bytes_per_row = 1,
                                       .first_char    = 0,
                                       .last_char     = 255};
const BitmapFont TRI89B_font_8x11   = {.glyphs        = ___vga_text_mode_fonts_FONTS_PC_OTHER_TRI89B_F11,
                                       .char_width    = 8,
                                       .char_height   = 11,
                                       .bytes_per_row = 1,
                                       .first_char    = 0,
                                       .last_char     = 255};
// Default active font
BitmapFont *active_font = (BitmapFont *)&TRI89B_font_8x11;

void        set_font(const BitmapFont *font) {
  if (font) {
    active_font = (BitmapFont *)font;
  } else {
    active_font = (BitmapFont *)&z100_font;
  }
}