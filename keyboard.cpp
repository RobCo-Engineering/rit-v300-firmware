#include <cstdio>
#include <iostream>

#include "etl/array.h"
#include "etl/vector.h"
#include "hardware/uart.h"
#include "keyboard.h"

State                    state = State::WAIT_H1;

etl::vector<uint8_t, 16> buffer;
etl::array<uint8_t, 8>   prev_report = {0};

Cmd                      cmd         = Cmd::KeyboardFrame;

// Convert a single HID usage to ASCII, considering modifiers
char hid_usage_to_ascii(uint8_t usage, uint8_t modifiers) {
  auto it = hid_to_ascii.find(usage);
  if (it != hid_to_ascii.end()) {
    char c = it->second;
    // simple Shift handling (letters only)
    if ((modifiers & 0x22) && c >= 'a' && c <= 'z') { // LShift or RShift
      c = c - 'a' + 'A';
    }
    return c;
  }
  return 0;
}

void handle_frame(Cmd cmd, const etl::vector<uint8_t, 16> &payload) {
  if (cmd != Cmd::KeyboardFrame) return; // keyboard only
  if (payload.size() < 8) return;

  // Compare key slots 2..7
  for (size_t i = 2; i < 8; ++i) {
    uint8_t key = payload[i];
    if (key == 0) continue;

    // Is key new? (not found in previous report)
    auto it = etl::find(prev_report.begin() + 2, prev_report.end(), key);
    if (it == prev_report.end()) {
      char ascii = hid_usage_to_ascii(key, payload[0]);
      if (ascii) {
        // printf("KeyDown: %c\n", ascii);
        uart_putc(uart1, ascii);
      }
    }
  }

  // Update previous report
  etl::copy(payload.begin(), payload.begin() + 8, prev_report.begin());
}

void ch9350_irq_handler() {
  while (uart_is_readable(uart0)) {
    uint8_t c = uart_getc(uart0);

    switch (state) {
    case State::WAIT_H1:
      if (c == 0x57) state = State::WAIT_H2;
      break;

    case State::WAIT_H2:
      if (c == 0xAB)
        state = State::CMD;
      else
        state = State::WAIT_H1;
      break;

    case State::CMD:
      cmd = static_cast<Cmd>(c);
      buffer.clear();
      if (cmd == Cmd::KeyboardFrame) {
        state = State::PAYLOAD;
      } else {
        // only handling 0x01 for keyboard data here, maybe we want more idk
        state = State::WAIT_H1;
      }
      break;

    case State::PAYLOAD:
      buffer.push_back(c);
      if (buffer.size() == 8) { // fixed 8 bytes for keyboard
        handle_frame(cmd, buffer);
        state = State::WAIT_H1;
      }
      break;
    }
  }
}
