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

void jobs()
{
  char *status;
  for(int i = 0; i < bg_num_children; ++i)
  {
    status = findStatus(bg_children_PIDs[i]);
    status[strlen(status)-2] = '\0';
    status[0] = toupper(status[0]);
    printf("[%d] %s %s[%d]\n", (i+1), status, children_name[bg_children_PIDs[i]], (int)bg_children_PIDs[i]);
  }
}
