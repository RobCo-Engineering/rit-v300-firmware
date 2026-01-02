
#include "etl/string.h"

constexpr size_t MAX_TOKENS  = 8;
constexpr size_t CMDLINE_MAX = 64;

struct Tokens {
  const char *argv[MAX_TOKENS];
  size_t      argc;
};

Tokens tokenize(const etl::string<CMDLINE_MAX> &line);

using cmd_fn_t = void (*)(const Tokens &);

struct Command {
  const char *name;
  cmd_fn_t    fn;
};

void handle_char(char c);
void cmd_run(const Tokens &t);
void cmd_set(const Tokens &t);
void execute_command(etl::string<CMDLINE_MAX> &line);