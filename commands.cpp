#include "commands.h"
#include "commands/termlink.cpp"
#include "etl/string.h"

etl::string<CMDLINE_MAX> cmdline;

// Command function declarations, note that the commands are suppose to look like a DEC VMS command line
constexpr Command command_table[] = {
    {"RUN", cmd_run},
    {"SET", cmd_set},
};

// Handle input character for command line
void handle_char(char c) {
  if (c == '\r' || c == '\n') {
    printf("\n");
    execute_command(cmdline);
    cmdline.clear();
    printf("> ");                      // prompt
  } else if (c == '\b' || c == 0x7F) { // backspace
    if (!cmdline.empty()) {
      cmdline.pop_back();
      printf("\b \b"); // erase on terminal
    }
  } else {
    if (cmdline.size() < CMDLINE_MAX - 1) {
      cmdline.push_back(c);
      printf("%c", c);
    }
  }
}

// Tokenize input line into arguments
Tokens tokenize(const etl::string<CMDLINE_MAX> &line) {
  Tokens t{};
  char  *s = const_cast<char *>(line.c_str());

  while (*s && t.argc < MAX_TOKENS) {
    while (*s == ' ')
      ++s; // skip spaces
    if (*s == '\0') break;

    t.argv[t.argc++] = s;

    while (*s && *s != ' ')
      ++s; // advance token
    if (*s) {
      *s = '\0'; // terminate token
      ++s;
    }
  }
  return t;
}

// Execute command based on tokenized input
void execute_command(etl::string<CMDLINE_MAX> &line) {
  Tokens t = tokenize(line);

  if (t.argc == 0) return;

  for (auto &cmd : command_table) {
    if (strcmp(t.argv[0], cmd.name) == 0) {

      // Start in another thread or core? Need to find a way to still accept input so user can abort
      // need to really just accept Ctrl-C or similar to abort long running commands, I don't know if
      // that should be done here or in the command implementations
      cmd.fn(t);
      return;
    }
  }

  printf("Unknown command '%s'\n", t.argv[0]);
}

// RUN command implementation
void cmd_run(const Tokens &t) {
  if (t.argc < 2) {
    printf("RUN: missing target\n");
    return;
  }

  const char *target = t.argv[1];

  if (strcmp(target, "TERMLINK.H") == 0) {
    run_termlink();
  } else {
    printf("RUN: unknown target '%s'\n", target);
  }
}

// SET command implementation
void cmd_set(const Tokens &t) {
  if (t.argc < 3) {
    printf("SET <key> <value>\n");
    return;
  }

  // Handle different keys and store values
  if (strcmp(t.argv[1], "BAUD") == 0) {
    int baud = atoi(t.argv[2]);
    printf("Setting baud to %d\n", baud);
    // uart_set_baudrate(uart0, baud);
  } else {
    printf("SET: unknown key '%s'\n", t.argv[1]);
  }
}
