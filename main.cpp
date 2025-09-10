#include <cstdio>
#include <math.h>
// #include <stdlib.h>

#include "memory.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <pico/binary_info.h>

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include <hardware/uart.h>

#include "cvideo.h"
#include "fonts.h"
#include "graphics.h"
#include "terminal.h"

#include "keyboard.h"
#include "main.h"

#include "vt_logo.c"

/*
void dump_dma_claims(void) {
  for (int i = 0; i < NUM_DMA_CHANNELS; ++i) {
    printf("DMA %02d claimed=%d\n", i, dma_channel_is_claimed(i));
  }
}

void dump_irq_priorities(void) {
  for (uint irq = 0; irq < 52; irq++) {
    uint prio = irq_get_priority(irq);
    printf("IRQ %2d priority=0x%02x\n", irq, prio);
  }
}
*/

const char uos[] = "Unified Operating System";
const char rob[] = "RobCo Industries 2074";

// Show a splash screen
void splash(const char *extra) {
  cls(0);

  set_border(col_black);

  print_string((width / 2) - ((strlen(uos) * active_font->char_width) / 2), 24, uos, DEFAULT_BG, DEFAULT_FG);
  print_string((width / 2) - ((strlen(rob) * active_font->char_width) / 2), 180, rob, DEFAULT_BG, DEFAULT_FG);
  print_string((width / 2) - ((strlen(extra) * active_font->char_width) / 2), 200, extra, DEFAULT_BG, DEFAULT_FG);

  draw_bitmap((380 - vt_logo_w) / 2, 70, vt_logo_w, vt_logo_h, vt_logo_bitmap, 0);
}

void main2() {
  printf("SETTING UP HID...\n");

  gpio_init(USBH_DPLUS);
  gpio_init(USBH_DMINUS);

  gpio_set_function(USBH_DPLUS, UART_FUNCSEL_NUM(uart0, 0));  // D+ = TX0
  gpio_set_function(USBH_DMINUS, UART_FUNCSEL_NUM(uart0, 1)); // D- = RX0

  uart_init(uart0, 115200);

  irq_set_exclusive_handler(UART0_IRQ, ch9350_irq_handler);
  irq_set_enabled(UART0_IRQ, true);
  uart_set_irq_enables(uart0, true, false);

  while (true) {
  }
}

// The main loop
int main() {

  bi_decl(bi_program_description("RobCo Industries RIT-V300 terminal"));
  bi_decl(bi_program_url("https://github.com/rikkuness/rit-v300-firmware"));
  bi_decl(bi_program_version_string("Build date and time: " __DATE__ " " __TIME__));
  // bi_decl(bi_2pins_with_func(PICO_DEFAULT_UART_RX_PIN, PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART));
  bi_decl(bi_2pins_with_func(USBH_DPLUS, USBH_DMINUS, GPIO_FUNC_UART));

  // stdio_driver_t screen_driver = {.out_chars = screen_out_chars, .in_chars = NULL, .crlf_enabled = NULL};

  stdio_init_all();

  // TODO: Pipe to display also
  // stdio_set_driver_enabled

  printf("\n================\nROBCO INDUSTRIES\nRIT-V300\n================\n");

  printf("INITIALIZING VIDEO...\n");
  initialise_cvideo(); // Initialise the composite video stuff
  printf("SETTING VIDEO MODE...\n");
  set_mode(4);              // Set video mode to 390 x 240
  splash("System booting"); // Display a splash screen

  // USB host power
  printf("ENABLING BUS POWER...\n");
  gpio_init(USBH_POWER_EN);
  gpio_set_dir(USBH_POWER_EN, GPIO_OUT);
  gpio_put(USBH_POWER_EN, 1);

  // On board LED
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  printf("INITIALIZING INTERFACE BUS...\n");

  // Launch USB management on core1
  multicore_launch_core1(main2);

  // printf("DUMPING DMA CLAIMS...\n");
  // dump_dma_claims();

  // printf("DUMPING IRQ PRIORITIES...\n");
  // dump_irq_priorities();

  // Mysterious 3 second pause so things don't appear so fast
  sleep_ms(1000 * 3);

  while (true) {
    terminal_app();
  }
}

// Simple terminal output from UART
//
void terminal_app(void) {
  printf("INITIALIZING TERMLINK...\n");
  initialise_terminal(); // Initialise the UART
  cls(DEFAULT_BG);       // Clear the screen
  terminal();            // And do the terminal
}