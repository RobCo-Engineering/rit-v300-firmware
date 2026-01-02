#include "cvideo.h"

void term_cmds() {
  // Live UART command parser for tuning video parameters
  static char linebuf[64];
  static int linepos = 0;
  static bool last_was_cr = false;
  int c = getchar_timeout_us(0);
  while (c != PICO_ERROR_TIMEOUT) {
    if (c == 8 || c == 127) { // backspace or DEL
      if (linepos > 0) {
        linepos--;
        // erase on terminal
        putchar('\b');
        putchar(' ');
        putchar('\b');
      }
      last_was_cr = false;
    } else if (c == 'o') {
      shift_clkdiv_fixed((int32_t)(0.05f * 65536));  

    } else if (c == 'p') {
      shift_clkdiv_fixed((int32_t)(-0.05f * 65536));
    } else if (c == '\r' || c == '\n' || linepos >= (int)sizeof(linebuf) - 1) {
      // handle CR/LF as line terminator; avoid double-processing CRLF pairs
      if (c == '\n' && last_was_cr) {
        // skip the LF that follows a CR
        last_was_cr = false;
        c = getchar_timeout_us(0);
        continue;
      }
      linebuf[linepos] = '\0';
      if (linepos > 0) {
        double dv;
        int mi;
        if (sscanf(linebuf, "mode %d", &mi) == 1) {

        } else {
          char arg[32];
          if (sscanf(linebuf, "test %31s", arg) == 1) {
            if (strcmp(arg, "v") == 0 || strcmp(arg, "vert") == 0 ||
                strcmp(arg, "vertical") == 0) {
              cls(0);
              colour_bars_v();
              printf("OK test vertical\n");
            } else if (strcmp(arg, "h") == 0 || strcmp(arg, "hor") == 0 ||
                       strcmp(arg, "horizontal") == 0) {
              cls(0);
              colour_bars_h();
              printf("OK test horizontal\n");
            } else if (strcmp(arg, "logo") == 0 ||
                       strcmp(arg, "pattern") == 0 ||
                       strcmp(arg, "indian") == 0) {
              cls(0);
              indian_head_test();
              printf("OK test logo\n");
            } else {
              printf("Unknown test: %s\n", arg);
            }
          } else {
            printf("Unknown command: %s\n", linebuf);
            printf("Commands: overscan <percent>, cycles <value>, mode <n>, "
                   "hslice <slices>, timing, test <v|h|logo>\n");
          }
        }
      }
      linepos = 0;
      last_was_cr = (c == '\r');
    } else {
      // echo and accumulate
      putchar((char)c);
      linebuf[linepos++] = (char)c;
      last_was_cr = false;
    }
    c = getchar_timeout_us(0);
  }
  tight_loop_contents();
}