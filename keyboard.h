#pragma once
#include "etl/map.h"

constexpr size_t                             HID_MAP_SIZE = 50;

inline etl::map<uint8_t, char, HID_MAP_SIZE> hid_to_ascii = {
    {0x04, 'a'}, {0x05, 'b'}, {0x06, 'c'},  {0x07, 'd'}, {0x08, 'e'},  {0x09, 'f'}, {0x0A, 'g'},  {0x0B, 'h'}, {0x0C, 'i'}, {0x0D, 'j'},
    {0x0E, 'k'}, {0x0F, 'l'}, {0x10, 'm'},  {0x11, 'n'}, {0x12, 'o'},  {0x13, 'p'}, {0x14, 'q'},  {0x15, 'r'}, {0x16, 's'}, {0x17, 't'},
    {0x18, 'u'}, {0x19, 'v'}, {0x1A, 'w'},  {0x1B, 'x'}, {0x1C, 'y'},  {0x1D, 'z'}, {0x1E, '1'},  {0x1F, '2'}, {0x20, '3'}, {0x21, '4'},
    {0x22, '5'}, {0x23, '6'}, {0x24, '7'},  {0x25, '8'}, {0x26, '9'},  {0x27, '0'}, {0x28, '\n'}, {0x2C, ' '}, {0x2D, '-'}, {0x2E, '='},
    {0x2F, '['}, {0x30, ']'}, {0x31, '\\'}, {0x33, ';'}, {0x34, '\''}, {0x36, ','}, {0x37, '.'},  {0x38, '/'}};

enum class State {
  WAIT_H1,
  WAIT_H2,
  CMD,
  PAYLOAD,
};

enum class Cmd : uint8_t {
  KeyboardFrame = 0x01,
  UsbUnplugged  = 0x86,
  StatusChanged = 0x80,
  Hid83         = 0x83,
  Hid88         = 0x88,
};
void ch9350_irq_handler();
