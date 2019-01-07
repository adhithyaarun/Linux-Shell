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

void ls(int argc, char* argv[])
{
  DIR* dir;
  int file = 0;
  int a_flag = 0;
  int l_flag = 0;
  char file_type[1];
  int permissions[9];
  char dirname[500005];
  char filename[500005];
  char details[5][5005];

  for(int i = 0; i < 9; ++i)
  {
    permissions[i] = 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    if(argv[i][0] == '-')
    {
      if((strcmp(argv[i], "-la") != 0) && (strcmp(argv[i], "-al") != 0) && (strcmp(argv[i], "-l") != 0) && (strcmp(argv[i], "-a") != 0))
      {
        fprintf(stderr, "ls: invalid option\n");
        return;
      }
    }
  }

  if((argc == 1) || ((argc > 1) && (argv[argc - 1][0] == '-')))
  {
    strcpy(dirname, PWD_ABS_NAME);
    dir = opendir(PWD_ABS_NAME);
    if(dir == NULL)
    {
      perror("ls");
      return;
    }
  }
  else
  {
    file = 1;
    if(argv[argc - 1][0] == '/')
    {
      strcpy(dirname, argv[argc - 1]);
    }
    else if(argv[argc - 1][0] == '~')
    {
      char temp[500005];
      strcpy(dirname, HOME);
      strcat(dirname, "/");
      argv[argc - 1][0] = '.';
      strcat(dirname, argv[argc - 1]);
    }
    else
    {
      strcpy(dirname, PWD_ABS_NAME);
      strcat(dirname, "/");
      strcat(dirname, argv[argc - 1]);
    }
    dir = opendir(dirname);
    if(dir == NULL)
    {
      char error[1001] = "ls: cannot access \'";
      strcat(error, argv[argc - 1]);
      strcat(error, "\'");
      perror(error);
      return;
    }
  }

  for(int i = 1; i < (argc - file); ++i)
  {
    for(int j = 0; j < strlen(argv[i]); ++j)
    {
      if(argv[i][j] == 'a')
      {
        a_flag = 1;
      }

      if(argv[i][j] == 'l')
      {
        l_flag = 1;
      }
    }
  }

  struct dirent* content = readdir(dir);
  if(content == NULL)
  {
    perror("Directory read error");
  }
  while(content != NULL)
  {
    for(int i = 0; i < 9; ++i)
    {
      permissions[i] = 0;
    }

    strcpy(filename, dirname);
    strcat(filename, "/");
    strcat(filename, content->d_name);
    int ret = check_permissions(permissions, details, file_type, filename);
    if(ret != 0)
    {
      int ret_close = closedir(dir);
      if(ret_close != 0)
      {
        perror("Directory close error");
      }
      return;
    }

    if((a_flag != 1) && (content->d_name[0] == '.'))
    {
      content = readdir(dir);
      continue;
    }
    else
    {
      if(l_flag == 1)
      {
        printf("%c", file_type[0]);
        for(int x = 0; x < 9; ++x)
        {
          if(permissions[x] == 1)
          {
            if(x % 3 == 0)
            {
              printf("r");
            }
            else if(x % 3 == 1)
            {
              printf("w");
            }
            else if(x % 3 == 2)
            {
              printf("x");
            }
          }
          else
          {
            printf("-");
          }
        }

        printf(" %s ", details[0]);
        printf("%s ", details[1]);
        printf("%s ", details[2]);
        printf("%9s ", details[3]);
        printf("%s ", details[4]);

        if(file_type[0] == 'd')
        {
          printf(" "BLUE"%s"RESET"\n", content->d_name);
        }
        else
        {
          printf(" %s\n", content->d_name);
        }
      }
      else if(l_flag != 1)
      {
        if(file_type[0] == 'd')
        {
          printf(BLUE"%s"RESET"\n", content->d_name);
        }
        else
        {
          printf("%s\n", content->d_name);
        }
      }
    }
    content = readdir(dir);
  }
  int ret_close = closedir(dir);
  if(ret_close != 0)
  {
    perror("Directory close error");
  }
}
