#include "prototype.h"

// Global variable declarations
struct utsname UNAME;
char* SYSTEM_NAME;
char HOME[500005];
char* USER;
char PWD_ABS_NAME[500005];
char PWD_NAME[500005] = "~";
DIR* PWD;
char* cmd;
char* BUILT_IN[] = {"quit", "ls", "echo", "clear", "cd", "pwd", "pinfo", "remindme", "clock", "setenv", "unsetenv", "overkill", "jobs", "kjob", "fg", "bg"};
int num_built_in = 16;
char children_name[100000][51];
pid_t bg_children_PIDs[1000];
int bg_num_children = 0;
pid_t currentChildPID = -1;
pid_t startShellPID = 0;
volatile sig_atomic_t run_clock = 1;

int main(int argc, char const *argv[])
{
  signal(SIGINT, ctrl_c);
  signal(SIGTSTP, ctrl_z);
  signal(SIGCHLD, sigchld);

  char* ret_val = NULL;
  int uname_status = uname(&UNAME);
  if(uname_status != 0)
  {
    perror("Fatal Error");
    exit(1);
  }

  // Home (set as current directory)
  ret_val = getcwd(HOME, 500005);
  if(ret_val == NULL)
  {
    perror("Fatal Error");
    exit(1);
  }
  ret_val = NULL;

  // Username
  USER = getenv("USER");
  if(USER == NULL)
  {
    perror("Fatal Error");
    exit(1);
  }

  // System name
  SYSTEM_NAME = UNAME.nodename;

  // Present directory
  ret_val = getcwd(PWD_ABS_NAME, 500005);
  if(ret_val == NULL)
  {
    perror("Fatal Error");
    exit(1);
  }

  // Opening the current directory
  PWD = opendir(PWD_ABS_NAME);
  if(PWD == NULL)
  {
    perror("Fatal Error");
  }

  // Getting PID of parent shell process that is initially launched
  startShellPID = getpid();

  run();

  return 0;
}

int tokenize(char* string, char delim[], char* tokens[])
{
  int count = 0;
  char* ptr;
  char* tok;

  ptr = string;
  while((tok = strtok(ptr, delim)) != NULL)
  {
    tokens[count++] = tok;
    ptr = NULL;
  }
  return count;
}

void run()
{
  int int_ret_value = 0;
  char* ret_val = NULL;
  char cmd[500005];
  char* commands[1000];
  char* commandsets[1000];
  char* tok;
  char* ptr;
  char* tok1;
  char* ptr1;
  int num_cmd = 0;
  int num_cmdsets = 0;
  size_t size = 500005;

  while(1)
  {
    currentChildPID = -1;
    ret_val = getcwd(PWD_ABS_NAME, 500005);
    if(ret_val == NULL)
    {
      perror("Fatal Error");
      exit(1);
    }

    currentChildPID = -1;
    ptr = NULL;
    PWD = opendir(PWD_ABS_NAME);

    if(PWD == NULL)
    {
      perror("Fatal Error");
    }

    display_path(PWD_ABS_NAME, PWD_NAME);

    printf("<"GREEN"%s@%s"RESET":"BLUE"%s"RESET">", USER, SYSTEM_NAME, PWD_NAME);
    ptr = &cmd[0];

    int_ret_value = getdelim(&ptr, &size, (int)'\n', stdin);
    if(int_ret_value == -1)
    {
      perror("Input Error");
      int_ret_value = 0;
      continue;
    }

    num_cmdsets = 0;

    ptr[strlen(ptr) - 1] = '\0';

    // Tokenize commands
    num_cmdsets = tokenize(ptr, ";", commandsets);

    // Tokenize each command
    for(int j = 0; j < num_cmdsets; ++j)
    {
      num_cmd = tokenize(commandsets[j], "|", commands);

      if(num_cmd > 1)
      {
        piping(commands, num_cmd);
        continue;
      }

      for(int i = 0; i < num_cmd; ++i)
      {
        run_cmd(commands[i]);
      }
    }
  }
  exit(0);
}

void run_cmd(char* command)
{
  char* ptr_ret_value = NULL;
  char* tok;
  char* ptr;
  char* argv[1000];
  char* argv1[1000];
  int argc = 0;
  int argc1 = 0;
  int pid;
  int* status;
  pid_t parent_pid;
  int ret_status;
  pid_t ret_pid;
  int clock_pid;
  int t;
  int inputFile = 0;
  int outputFile = 1;

  argc = 0;
  ptr = command;
  argc1 = tokenize(command, " \t", argv1);

  int initial_input = dup(0);
  int initial_output = dup(1);
  int redirectInput = 0;
  int redirectOutput = 0;
  int redirectAppend = 0;
  char inputFileName[101];
  char outputFileName[101];
  char appendFileName[101];
  int flag = 0;

  for(int i = 0; i < argc1; ++i)
  {
    if(!strcmp(argv1[i], "<"))
    {
      flag = 1;
      if(i == (argc1-1))
      {
        fprintf(stderr, "Error is usage of redirect, no filename provided\n");
        return;
      }
      else
      {
        redirectInput = 1;
        strcpy(inputFileName, argv1[++i]);
      }
    }
    else if(!strcmp(argv1[i], ">"))
    {
      flag = 1;
      if(i == (argc1-1))
      {
        fprintf(stderr, "Error is usage of redirect, no filename provided\n");
        return;
      }
      else
      {
        redirectOutput = 1;
        redirectAppend = 0;
        strcpy(outputFileName, argv1[++i]);
      }
    }
    else if(!strcmp(argv1[i], ">>"))
    {
      flag = 1;
      if(i == (argc1-1))
      {
        fprintf(stderr, "Error is usage of redirect, no filename provided\n");
        return;
      }
      else
      {
        redirectAppend = 1;
        redirectOutput = 0;
        strcpy(appendFileName, argv1[++i]);
      }
    }
    if(!flag)
    {
      argv[argc++] = argv1[i];
    }
  }

  if(redirectInput)
  {
    inputFile = open(inputFileName, O_RDONLY);
    if(inputFile < 0)
    {
      fprintf(stderr, "Please provide a valid filename for redirection");
      return;
    }
    dup2(inputFile, 0);
  }

  if(redirectOutput)
  {
    outputFile = open(outputFileName, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(outputFile < 0)
    {
      fprintf(stderr, "Please provide a valid filename for redirection");
      return;
    }
    dup2(outputFile, 1);
  }

  else if(redirectAppend)
  {
    outputFile = open(appendFileName, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if(outputFile < 0)
    {
      fprintf(stderr, "Please provide a valid filename for redirection");
      return;
    }
    dup2(outputFile, 1);
  }

  // check for BUILT IN commands
  int l = 0;
  for(l = 0; l < num_built_in; ++l)
  {
    if(!strcmp(BUILT_IN[l], argv[0]))
    {
      break;
    }
  }

  // If BUILT IN command, execute apt function
  if(l < num_built_in)
  {
    switch(l)
    {
      case 0:
              if(argc > 1)
              {
                fprintf(stderr, "quit takes no arguments\n");
                break;
              }
              exit_shell();
              break;
      case 1:
              if(argc > 2)
              {
                fprintf(stderr, "Invalid usage\nUsage: ls <dirname>\n");
                break;
              }
              ls(argc, argv);
              break;
      case 2:
              echo(argc, argv);
              break;
      case 3:
              if(argc > 1)
              {
                fprintf(stderr, "clear takes no arguments\n");
                break;
              }
              clear();
              break;
      case 4:
              if(argc > 2)
              {
                fprintf(stderr, "Invalid usage\nUsage: cd <dirname>\n");
                break;
              }
              cd(argc, argv);
              break;
      case 5:
              if(argc > 1)
              {
                fprintf(stderr, "pwd takes no arguments\n");
                break;
              }
              pwd();
              break;
      case 6:
              pinfo(argc, argv);
              break;
      case 7:
              remindme(argc, argv);
              break;
      case 8:
              if(argc < 3)
              {
                fprintf(stderr, "Invalid usage\nUsage: clock -t <interval>\n");
                if(redirectInput)
                {
                  close(inputFile);
                  dup2(initial_input, 0);
                  inputFile = -1;
                }
                if(redirectOutput || redirectAppend)
                {
                  close(outputFile);
                  dup2(initial_output, 1);
                  outputFile = -1;
                }
                return;
              }
              else
              {
                clock_pid = fork();
                if(clock_pid == 0)
                {
                  t = atoi(argv[2]);
                  clockcmd(t);
                }
                strcpy(children_name[clock_pid], "clock");
                waitpid(clock_pid, status, 0);
              }
              break;
      case 9:
              if(argc != 3)
              {
                fprintf(stderr, "Invalid usage\nUsage: setenv <var> <[value]>\n");
                break;
              }
              setEnv(argc, argv);
              break;
      case 10:
              if(argc != 2)
              {
                fprintf(stderr, "Invalid usage\nUsage: unsetenv <var>\n");
                break;
              }
              unsetEnv(argc, argv);
              break;
      case 11:
              if(argc > 1)
              {
                fprintf(stderr, "overkill takes no arguments\n");
                break;
              }
              overkill();
              break;
      case 12:
              if(argc > 1)
              {
                fprintf(stderr, "jobs takes no arguments\n");
                break;
              }
              jobs();
              break;
      case 13:
              if(argc != 3)
              {
                fprintf(stderr, "Invalid usage\nUsage: kjob <jobNumber> <signalNumber>\n");
              }
              else
              {
                kjob( atoi(argv[1]), atoi(argv[2]));
              }
              break;
      case 14:
              if(argc != 2)
              {
                fprintf(stderr, "Invalid usage\n Usage: fg <jobNumber>\n");
              }
              else
              {
                fg( atoi(argv[1]));
              }
              break;
      case 15:
              if(argc != 2)
              {
                fprintf(stderr, "Invalid usage\n Usage: bg <jobNumber>\n");
              }
              else
              {
                bg( atoi(argv[1]));
              }
              break;
      default:
              break;
    }
  }
  else
  {
    pid = fork();

    if(pid != 0)
    {
      strcpy(children_name[pid], argv[0]);
      if((!strcmp(argv[argc - 1], "&")))
      {
        addChildBg(pid);
        if(redirectInput)
        {
          close(inputFile);
          dup2(initial_input, 0);
          inputFile = -1;
        }
        if(redirectOutput || redirectAppend)
        {
          close(outputFile);
          dup2(initial_output, 1);
          outputFile = -1;
        }
        return;
      }
      else
      {
        currentChildPID = pid;
        waitpid(pid, status, WUNTRACED);
        if(redirectInput)
        {
          close(inputFile);
          dup2(initial_input, 0);
          inputFile = -1;
        }
        if(redirectOutput || redirectAppend)
        {
          close(outputFile);
          dup2(initial_output, 1);
          outputFile = -1;
        }
      }
    }
    else if(pid == 0)
    {
      if((!strcmp(argv[argc - 1], "&")))
      {
        if(setpgid(0, 0) != 0)
        {
          fprintf(stderr ,"Cannot start background process\n");
        }
        argv[argc - 1] = NULL;
        --argc;
      }

      if(execvp(argv[0], argv) == -1)
      {
        perror("Error");
        exit(1);
      }
    }
  }
  if(redirectInput)
  {
    close(inputFile);
    dup2(initial_input, 0);
    inputFile = -1;
  }
  if(redirectOutput || redirectAppend)
  {
    close(outputFile);
    dup2(initial_output, 1);
    outputFile = -1;
  }
}

void addChildBg(pid_t pid)
{
  bg_children_PIDs[bg_num_children++] = pid;
}

void removeChildBg(int index)
{
  for(int i = index; i < (bg_num_children-1); ++i)
  {
    bg_children_PIDs[i] = bg_children_PIDs[i+1];
  }
  --bg_num_children;
}

void display_path(char PWD_ABS_NAME[], char PWD_NAME[])
{
  if(strlen(HOME) > strlen(PWD_ABS_NAME))
  {
    strcpy(PWD_NAME, PWD_ABS_NAME);
    return;
  }
  else if(!strcmp(HOME, PWD_ABS_NAME))
  {
    strcpy(PWD_NAME, "~");
    return;
  }

  int flag = 1;
  int i = 0;
  for(i = 0; i < strlen(HOME); ++i)
  {
    if(PWD_ABS_NAME[i] != HOME[i])
    {
      flag = 0;
      break;
    }
  }

  if(flag == 1)
  {
    strcpy(PWD_NAME, "~");
    char letter[1];
    for(; i < strlen(PWD_ABS_NAME); ++i)
    {
      letter[0] = PWD_ABS_NAME[i];
      strcat(PWD_NAME, letter);
    }
  }
  else
  {
    strcpy(PWD_NAME, PWD_ABS_NAME);
    return;
  }
}

void clear()
{
  printf("\e[1;1H\e[2J");
}

void int_to_string(long long int val, char str[])
{
  long long int count = 0;
  long long int digit = 0;
  char temp[100];
  strcpy(str, "");
  while(val > 0)
  {
    digit = val % 10;
    val = val / 10;
    if(count == 0)
    {
      if(digit == 0){ strcpy(str, "0"); }
      if(digit == 1){ strcpy(str, "1"); }
      if(digit == 2){ strcpy(str, "2"); }
      if(digit == 3){ strcpy(str, "3"); }
      if(digit == 4){ strcpy(str, "4"); }
      if(digit == 5){ strcpy(str, "5"); }
      if(digit == 6){ strcpy(str, "6"); }
      if(digit == 7){ strcpy(str, "7"); }
      if(digit == 8){ strcpy(str, "8"); }
      if(digit == 9){ strcpy(str, "9"); }
    }
    else
    {
      if(digit == 0){ temp[0] = '0'; }
      if(digit == 1){ temp[0] = '1'; }
      if(digit == 2){ temp[0] = '2'; }
      if(digit == 3){ temp[0] = '3'; }
      if(digit == 4){ temp[0] = '4'; }
      if(digit == 5){ temp[0] = '5'; }
      if(digit == 6){ temp[0] = '6'; }
      if(digit == 7){ temp[0] = '7'; }
      if(digit == 8){ temp[0] = '8'; }
      if(digit == 9){ temp[0] = '9'; }
      temp[1] = '\0';
      strcat(temp, str);
      strcpy(str, temp);
    }
    ++count;
  }
}

void ctrl_c(int signum)
{
  pid_t currentPID = getpid();
  if(startShellPID != currentPID)
  {
    return;
  }

  if(currentChildPID != -1)
  {
    kill(currentChildPID, SIGINT);
  }
  signal(SIGINT, ctrl_c);
}

void ctrl_z(int signum)
{
  pid_t currentPID = getpid();
  if(currentPID != startShellPID)
  {
    return;
  }

  if(currentChildPID != -1)
  {
    kill(currentChildPID, SIGTTIN);
    kill(currentChildPID, SIGTSTP);
    addChildBg(currentChildPID);
  }
  signal(SIGTSTP, ctrl_z);
}

void sigchld(int signum)
{
  int bgpid;
  int exit_check;
  int exit_status;
  for(int i = 0; i < bg_num_children; ++i)
  {
    bgpid = waitpid(bg_children_PIDs[i], &exit_check, WNOHANG);
    if(bgpid == bg_children_PIDs[i])
    {
      exit_status = WIFEXITED(exit_check);
      if(!strcmp(children_name[bgpid], "remindme") || !strcmp(children_name[bgpid], "clock"))
      {
        continue;
      }
      if(exit_status)
      {
        printf("\n%s with pid %d exited normally\n", children_name[bgpid], bgpid);
      }
      else
      {
        printf("\n%s with pid %d did not exit normally\n", children_name[bgpid], bgpid);
      }
      removeChildBg(i);
      --i;
    }
  }
}

void piping(char* commands[], int num_commands)
{
  int argc = 0;
  char* argv[1000];
  int fd[2];
  pid_t pid;
  int fd1 = 0;
  for(int i = 0; i < num_commands; ++i)
  {
    pipe(fd);
    pid = fork();
    if(pid == -1)
    {
      perror("Piping error");
      return;
    }
    else if(pid == 0)
    {
      dup2(fd1, 0);
      if(i != (num_commands - 1))
      {
        dup2(fd[1], 1);
      }
      close(fd[0]);
      run_cmd(commands[i]);
      exit(1);
    }
    else
    {
      wait(NULL);
      close(fd[1]);
      fd1 = fd[0];
    }
  }
}

char* findStatus(pid_t pid)
{
  char* tok;
  char* ptr;
  char pid_str[10];
  FILE* file;
  char status_file[505] = "/proc/";
  int_to_string((int)pid, pid_str);
  strcat(status_file, pid_str);
  strcat(status_file, "/status");

  file = fopen(status_file, "r");
  if(file == NULL)
  {
    perror("Error");
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int count = 1;
  int k = 0;


  while((read = getline(&line, &len, file)) != -1)
  {
    if(count == 3)
    {
      k = 0;
      ptr = line;
      while((tok = strtok(ptr, " \t")) != NULL)
      {
        k++;
        ptr = NULL;
        if(k > 1)
        {
          if(k == 2)
          {
            ;
          }
          else
          {
            break;
          }
        }
      }
      break;
    }
    ++count;
  }
  return (tok+1);
}
