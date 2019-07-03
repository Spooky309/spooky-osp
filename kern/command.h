#ifndef _H_COMMAND_
#define _H_COMMAND_
#define CMDS_MAX 128
typedef struct command {
  char* cmdText;
  char* description;
  void (*execute)(const char* args);
} command_t;
command_t commands[CMDS_MAX];
void register_command(char* cmdText, char* description, void* execFunc);
void initCmds();
#endif