#include "term.h"
#include "command.h"
#include "ckern.h"
#include "string.h"
#include "mem.h"
unsigned int commandIt = 0;
/* REGISTER COMMAND:
 * cmdText is the text used to invoke the command
 * description is the text displayed when the "help" command is used. Make it short.
 *   (if description is "hidden" then the command will not be displayed in help)
 * execFunc is a pointer to a function that takes a const char*
 */
 
void register_command(char* cmdText, char* description, void* execFunc) {
  if (commandIt < CMDS_MAX) {
    commands[commandIt].cmdText = cmdText;
    commands[commandIt].execute = execFunc;
    commands[commandIt].description = description;
    commandIt++;
  }
  else {
    writeLine("Attempted to register a command past limit.\nPlease increase limit.");
  }
}
void cmdAnime() {
  panic(65536);
}
void cmdHelp() {
  writeLine("Note: everything is case sensitive");
  writeLine("Supported commands:");
  for (unsigned int i = 0; i < commandIt; i++) {
    if (strcmp(commands[i].description, "hidden") != 0) {
      puts(commands[i].cmdText);
      puts(": ");
      writeLine(commands[i].description);
    }
  }
}
void cmdWarranty() {
  writeLine("Not implemented here yet.");
  writeLine("check https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt for info");
}
void cmdAbout() {
  puts("ligmOS ");
  puts(VERSION);
  writeLine(" by Kole.");
  puts("...");
  switch (sysclock % 100) {
    case 0:  writeLine("Do you *feel* in control?"); break;
    case 1:  writeLine("I'll see you on the dark side of the moon."); break;
    case 2:  writeLine("Is this thing on?"); break;
    case 3:  writeLine("OwO what's this?"); break;
    case 4:  writeLine("You're a big guy."); break;
    case 5:  writeLine("How was your day? Don't tell me."); break;
    case 6:  writeLine("Don't cross the crooked step."); break;
    case 7:  writeLine("And all things will end."); break;
    case 8:  writeLine("SPEAK ONLY WHEN SPOKEN TO."); break;
    case 9:  writeLine("History is written by the victor... and we sure won."); break;
    case 10: writeLine("I've been told these messages aren't professional. To be fair, neither am I."); break;
    case 11: writeLine("All work and no play makes Jack a dull boy."); break;
    default: writeLine("The world's most useless operating system");
  }
}
void cmdKern() {
  // all i wanted was to feel at home.
  for (int i = 0; i < 6000; i++) {
    if (i == 4000) 
      ATTRIB = 0x40;
    puts("32alloc_tkerN MODE isNOT ");
  } 
  panic(65535);
}
void initCmds() {
  register_command("warranty", "display warranty information", cmdWarranty);
  register_command("license", "display license information", cmdWarranty);
  register_command("help", "this command", cmdHelp);
  register_command("about", "information", cmdAbout);
  register_command("kern", "warning", cmdKern);
  register_command("kawaii", "remember what i told you", cmdAnime);
  register_command("desu", "hidden", cmdAnime);
  register_command("senpai", "hidden", cmdAnime);
}
