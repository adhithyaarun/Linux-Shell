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

void setEnv(int argc, char* argv[])
{
  int ret = 0;
  if(argc != 3)
  {
    fprintf(stderr, "Error: Invalid usage of setenv\n");
    return;
  }
  else
  {
    ret = setenv(argv[1], argv[2], 1);

    if(ret != 0)
    {
      perror("setenv");
      return;
    }
  }
}

void unsetEnv(int argc, char* argv[])
{
  int ret = 0;
  if(argc != 2)
  {
    fprintf(stderr, "Error: Invalid usage of unsetenv\n");
    return;
  }
  else
  {
    ret = unsetenv(argv[1]);
    if(ret != 0)
    {
      perror("unsetenv");
      return;
    }
  }
}
