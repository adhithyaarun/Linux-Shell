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

void remindme(int argc, char* argv[])
{
  int pid = fork();
  if(pid == 0)
  {
    unsigned int timer = atoi(argv[1]);
    sleep(timer);
    printf("\nReminder: ");
    for(int i = 2; i < argc; ++i)
    {
      printf("%s ", argv[i]);
    }
    printf("\n");
    display_path(PWD_ABS_NAME, PWD_NAME);
    printf("<"GREEN"%s@%s"RESET":"BLUE"%s"RESET">", USER, SYSTEM_NAME, PWD_NAME);
    exit(0);
  }
  else if(pid < 0)
  {
    perror("Error in fork");
  }
  else
  {
    strcpy(children_name[pid], "remindme");
  }
}
