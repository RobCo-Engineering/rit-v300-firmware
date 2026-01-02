#include <stdlib.h>

#include "pico/stdio.h"
#include "pico/stdio/driver.h"

#include "pico/stdlib.h"

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/uart.h"

#include "cvideo.h"
#include "fonts.h"
#include "graphics.h"
#include "main.h"
#include "terminal.h"

typedef enum { STATE_NORMAL, STATE_ESC, STATE_CSI } terminal_parse_state_t;

typedef struct {
  int                    terminal_x;
  int                    terminal_y;

  uint16_t               col_fg;
  uint16_t               col_bg;

  terminal_parse_state_t state;
  char                   ansi_buf[8];
  int                    ansi_len;
} terminal_state_t;

static terminal_state_t term;

void                    initialise_terminal(void) {
  uart_init(uart1, UART_SPEED);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // RX
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // TX
}

/* ---------- cursor helpers (unchanged logic) ---------- */

static void cr(void) {
  term.terminal_x = LEFT_MARGIN;
  term.terminal_y += active_font->char_height + font_line_spacing;

  if (term.terminal_y >= height) {
    term.terminal_y -= active_font->char_height + font_line_spacing;
    scroll_up(DEFAULT_BG, active_font->char_height + font_line_spacing);
  }
}

static void fs(void) {
  term.terminal_x += active_font->char_width;
  if (term.terminal_x >= width) {
    cr();
  }
}

static void bs(void) {
  term.terminal_x -= active_font->char_width;
  if (term.terminal_x < LEFT_MARGIN) {
    term.terminal_x = LEFT_MARGIN;
  }
}

/* ---------- ANSI / character consumer ---------- */

static void terminal_putc(char c) {
  switch (term.state) {

  case STATE_NORMAL:
    if (c == 0x1B) {
      term.state = STATE_ESC;
    } else if (c >= 32) {
      print_char(term.terminal_x, term.terminal_y, c, term.col_bg, term.col_fg);
      fs();
    } else {
      switch (c) {
      case '\b':
        bs();
        break;
#if PICO_STDIO_ENABLE_CRLF_SUPPORT
      case '\r':
        cr();
        break;
      case '\n':
        break;
#else
      case '\r':
      case '\n':
        cr();
        break;
#endif

      default:
        break;
      }
    }
    break;

  case STATE_ESC:
    if (c == '[') {
      term.state    = STATE_CSI;
      term.ansi_len = 0;
    } else {
      term.state = STATE_NORMAL;
    }
    break;

  case STATE_CSI:
    if ((c >= '0' && c <= '9') || c == ';') {
      if (term.ansi_len < (int)sizeof(term.ansi_buf) - 1) {
        term.ansi_buf[term.ansi_len++] = c;
      }
    } else {
      term.ansi_buf[term.ansi_len] = '\0';

      if (c == 'm') {
        char *p = term.ansi_buf;
        while (*p) {
          int code = atoi(p);

          switch (code) {
          case 0:
            term.col_fg = DEFAULT_FG;
            term.col_bg = DEFAULT_BG;
            break;
          case 7: {
            uint16_t t  = term.col_fg;
            term.col_fg = term.col_bg;
            term.col_bg = t;
          } break;
          case 1:
            term.col_fg = GRAY16(12);
            break;
          case 2:
            term.col_fg = GRAY16(4);
            break;

          case 30 ... 37:
            term.col_fg = GRAY16((code - 30) * 2);
            break;
          case 40 ... 47:
            term.col_bg = GRAY16((code - 40) * 2);
            break;
          }

          while (*p && *p != ';')
            p++;
          if (*p == ';') p++;
        }
      }

      term.state = STATE_NORMAL;
    }
    break;
  }
}

/* ---------- stdio driver ---------- */

static void terminal_out_chars(const char *buf, int len) {
  for (int i = 0; i < len; i++) {
    terminal_putc(buf[i]);
  }
}

typedef struct stdio_driver stdio_driver_t;
static stdio_driver         terminal_stdio_driver = {
            .out_chars = terminal_out_chars,
};

/* ---------- public init ---------- */

void stdio_terminal_init(void) {
  term.terminal_x = LEFT_MARGIN;
  term.terminal_y = 0;
  term.col_fg     = DEFAULT_FG;
  term.col_bg     = DEFAULT_BG;
  term.state      = STATE_NORMAL;
  term.ansi_len   = 0;

  stdio_set_driver_enabled(&terminal_stdio_driver, true);
  stdio_set_translate_crlf(&terminal_stdio_driver, true);
}

void terminal_uart_task(void) {
  if (uart_is_readable(uart1)) {
    char c = uart_getc(uart1);
    terminal_putc(c);
  }
}
