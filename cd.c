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

void cd(int argc, char* argv[])
{
  if(!strcmp(argv[1], "."))
  {
    return;
  }
  else if((argc == 1) || !strcmp(argv[1], "~"))
  {
    int ret = chdir(HOME);
    if(ret != 0)
    {
      char error[1000] = "bash: cd: ";
      strcat(error, argv[1]);
      perror(error);
      return;
    }
  }
  else
  {
    char dirname[500005];
    if(argv[1][0] == '/')
    {
      strcpy(dirname, argv[1]);
    }
    else if(argv[1][0] == '~')
    {
      strcpy(dirname, HOME);
      strcat(dirname, "/");
      argv[1][0] = '.';
      strcat(dirname, argv[1]);
    }
    else
    {
      strcpy(dirname, PWD_ABS_NAME);
      strcat(dirname, "/");
      strcat(dirname, argv[1]);
    }
    int ret = chdir(dirname);
    if(ret != 0)
    {
      char error[1000] = "bash: cd: ";
      strcat(error, argv[1]);
      perror(error);
      return;
    }
  }
}
