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

void echo(int argc, char* argv[])
{
  char string[500005];
  char ic = 0;
  char quote;
  int start = -1;
  int end = -1;
  int count = 0;

  for(int i = 1; i < argc; ++i)
  {
    for(int j = 0; j < strlen(argv[i]); ++j)
    {
      if(argv[i][j] == '\'' || argv[i][j] == '\"')
      {
        if(count == 0)
        {
          start = i;
          quote = argv[i][j];
          count = (count + 1) % 2;
        }
        else if(argv[i][j] == quote)
        {
          end = i;
          count = (count + 1) % 2;
        }
        else
        {
          string[ic++] = argv[i][j];
        }
      }
      else
      {
        string[ic++] = argv[i][j];
      }
    }
    string[ic++] = ' ';
  }
  string[ic] = '\0';
  if(count != 0)
  {
    // Continue accepting input
    fprintf(stderr, "Invalid input\n");
    return;
  }
  printf("%s\n", string);
}
