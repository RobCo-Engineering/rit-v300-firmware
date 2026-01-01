#include "main.h"
#include "cvideo.h"
#include "graphics.h"
#include "RCA_Indian_Head_Test_Pattern.c"

// Test images - vertical bars in 16 colours
void colour_bars_v() {
  cls(0);

  const int rectw = width / 16;

  cls(col_black);
  for (int i = 0; i < 16; i++) {
    draw_rect(i * rectw, 0, i * rectw + rectw - 1, height, i, true);
  }
}

// Test images - horizontal bars in 16 colours
void colour_bars_h() {
  cls(0);
  
  const int recth = height / 16;
  for (int i = 0; i < 16; i++) {
    draw_rect(0, i * recth, width, i * recth + recth - 1, i, true);
  }
}

void indian_head_test() {
  cls(0);
  draw_bitmap(0, 0, RCA_Indian_Head_Test_Pattern_w,
              RCA_Indian_Head_Test_Pattern_h,
              RCA_Indian_Head_Test_Pattern_bitmap, 0);
}
