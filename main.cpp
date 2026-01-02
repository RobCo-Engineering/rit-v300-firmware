#include <cstdio>
#include <math.h>

#include "memory.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/uart.h"

#include "cvideo.h"
#include "cvideo_test.c"
#include "fonts.h"
#include "graphics.h"
#include "keyboard.h"
#include "main.h"
#include "terminal.h"
#include "uart_cmds.c"
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

const char rob[] = "RobCo Industries 2074";

// Show a splash screen
void splash() {
  cls(DEFAULT_BG); // Clear the screen
  set_border(col_black);
  draw_bitmap(65, 90, vt_logo_w, vt_logo_h, vt_logo_bitmap, 0); // Draw the VT logo
  print_string((width / 2) - ((strlen(rob) * active_font->char_width) / 2), 180, rob, DEFAULT_BG, DEFAULT_FG);
}

void main2() {
  gpio_init(USBH_DPLUS);
  gpio_init(USBH_DMINUS);

  gpio_set_function(USBH_DPLUS, UART_FUNCSEL_NUM(uart0, 0));  // D+ = TX0
  gpio_set_function(USBH_DMINUS, UART_FUNCSEL_NUM(uart0, 1)); // D- = RX0

  uart_init(uart0, 115200);

  irq_set_exclusive_handler(UART0_IRQ, ch9350_irq_handler);
  irq_set_enabled(UART0_IRQ, true);
  uart_set_irq_enables(uart0, true, false);

  while (true) {
    tight_loop_contents();
  }
}

// The main loop
int main() {

  bi_decl(bi_program_description("RobCo Industries RIT-V300 terminal"));
  bi_decl(bi_program_url("https://github.com/rikkuness/rit-v300-firmware"));
  bi_decl(bi_program_version_string("Build date and time: " __DATE__ " " __TIME__));
  // bi_decl(bi_2pins_with_func(PICO_DEFAULT_UART_RX_PIN, PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART));
  bi_decl(bi_2pins_with_func(USBH_DPLUS, USBH_DMINUS, GPIO_FUNC_UART));

  stdio_init_all();      // Initialize all default stdio types
  initialise_cvideo();   // Initialise the composite video stuff
  stdio_terminal_init(); // Initialise the terminal stdio driver to display on CVIDEO

  splash();           // Display a splash screen
  sleep_ms(1000 * 3); // Mysterious 3 second pause so things don't appear so fast
  cls(DEFAULT_BG);    // Clear the screen

  printf("\n\nInitializing Robco Industries(TM) MF Boot Agent v2.3.0\nRETROS BIOS\nRBIOS-4.02.08.00 52EE5.E7.E8\nCopyright 2201-2203 Robco "
         "Ind.\nUppermem: 64 KB\nRoot (5A8)\n\n");

  sleep_ms(2000); // Pause for dramatic effect

  // USB host power
  printf("Enabling bus power\n");
  gpio_init(USBH_POWER_EN);
  gpio_set_dir(USBH_POWER_EN, GPIO_OUT);
  gpio_put(USBH_POWER_EN, 1);

  // On board LED config
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  sleep_ms(500); // More pausing for dramatic effect
  printf("Initializing interface bus\n");

  // Launch USB management on core1
  sleep_ms(1000); // Even more pausing for dramatic effect
  multicore_launch_core1(main2);
  printf("READY.\n> "); // Prompt

  while (true) {
    terminal_uart_task();
  }
}
