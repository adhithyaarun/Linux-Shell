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

int check_permissions(int perm[], char details[][5005], char ftype[],char name[])
{
  char now_time[25];
  struct stat buffer;
  int stat_ret = stat(name, &buffer);
  if(stat_ret != 0)
  {
    perror("Directory access error");
    return 1;
  }
  mode_t permission = buffer.st_mode;

  if(permission & S_IRUSR)
  {
    perm[0] = 1;
  }
  if(permission & S_IWUSR)
  {
    perm[1] = 1;
  }
  if(permission & S_IXUSR)
  {
    perm[2] = 1;
  }
  if(permission & S_IRGRP)
  {
    perm[3] = 1;
  }
  if(permission & S_IWGRP)
  {
    perm[4] = 1;
  }
  if(permission & S_IXGRP)
  {
    perm[5] = 1;
  }
  if(permission & S_IROTH)
  {
    perm[6] = 1;
  }
  if(permission & S_IWOTH)
  {
    perm[7] = 1;
  }
  if(permission & S_IXOTH)
  {
    perm[8] = 1;
  }

  if(S_ISREG(permission))
  {
    ftype[0] = '-';
  }
  else if(S_ISDIR(permission))
  {
    ftype[0] = 'd';
  }
  else if(S_ISLNK(permission))
  {
    ftype[0] = 'l';
  }
  else
  {
    ftype[0] = '?';
  }

  int_to_string((long long int)buffer.st_nlink, details[0]);
  strcpy(details[1], (getpwuid(buffer.st_uid))->pw_name);
  strcpy(details[2], (getgrgid(buffer.st_gid))->gr_name);
  int_to_string((long long int)buffer.st_size, details[3]);
  strftime(now_time, 25, "%h %d %H:%M", localtime(&buffer.st_mtime));
  strcpy(details[4], now_time);

  return 0;
}

void pinfo(int argc, char* argv[])
{
  char* tok;
  char* ptr;
  char* status_args[5];
  char* vmsize_args[5];
  char vmsize[25];
  char status[25];
  char buf[500005];
  size_t bufsize = 500005;
  int pid;
  FILE* file;
  char status_file[505] = "/proc/";
  char path[505] = "/proc/";
  if(argc == 1)
  {
    pid = getpid();
    strcat(path, "self");
    strcat(status_file, "self");
  }
  else
  {
    pid = atoi(argv[1]);
    strcat(path, argv[1]);
    strcat(status_file, argv[1]);
  }
  strcat(path, "/exe");
  strcat(status_file, "/status");

  printf(BLUE"PID"RESET"%14s"CYAN"\t%d"RESET"\n", " ", pid);

  // Virtual Memory and status
  file = fopen(status_file, "r");

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int count = 1;
  int k = 0;


  while((read = getline(&line, &len, file)) != -1)
  {
    if(count == 3)
    {
      printf(BLUE"Process Status"RESET"   ");
      k = 0;
      ptr = line;
      while((tok = strtok(ptr, " \t")) != NULL)
      {
        status_args[k++] = tok;
        ptr = NULL;
        if(k > 1)
        {
          if(k == 2)
          {
            printf("\t"CYAN"%s"RESET, status_args[k-1]);
          }
          else
          {
            printf(" "CYAN"%s"RESET, status_args[k-1]);
          }
        }
      }
    }
    if(count == 18)
    {
      printf(BLUE"Virtual Memory"RESET"   ");
      k = 0;
      ptr = line;
      while((tok = strtok(ptr, " \t")) != NULL)
      {
        vmsize_args[k++] = tok;
        ptr = NULL;
        if(k > 1)
        {
          if(k == 2)
          {
            printf("\t"CYAN"%s"RESET, vmsize_args[k-1]);
          }
          else
          {
            printf(" "CYAN"%s"RESET, vmsize_args[k-1]);
          }
        }
      }
    }
    ++count;
  }

  strcpy(status, status_args[1]);
  strcat(status, status_args[2]);

  strcpy(vmsize, vmsize_args[1]);
  strcat(vmsize, vmsize_args[2]);

  // Executable
  for(int i = 0; i < bufsize; ++i)
  {
    buf[i] = '\0';
  }

  int ret = readlink(path, buf, bufsize);
  if(ret == -1)
  {
    perror("Readlink error");
    return;
  }

  for(int i = 0; i < 505; ++i)
  {
    path[i] = '\0';
  }
  display_path(buf, path);

  printf(BLUE"Executable path"RESET" "CYAN"\t%s"RESET"\n", path);
}
