#include "prototype.h"

extern struct utsname UNAME;
extern char* SYSTEM_NAME;
extern char HOME[500005];
extern char* USER;
extern char PWD_ABS_NAME[500005];
extern char PWD_NAME[500005];
extern DIR* PWD;
extern char* cmd;
extern int num_built_in;
extern char children_name[100000][51];
extern pid_t bg_children_PIDs[1000];
extern int bg_num_children;
extern pid_t currentChildPID;
extern pid_t startShellPID;
extern volatile sig_atomic_t run_clock;

void clockcmd(int t)
{
  run_clock = 1;
  char display_time[21];
  time_t base;

  while(run_clock)
  {
    time(&base);
    strftime(display_time, 21, "%d %b %y, %H:%M:%S\n", localtime(&base));
    printf("%s", display_time);
    sleep(t);
  }
  exit(0);
}