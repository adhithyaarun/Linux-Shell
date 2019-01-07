#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/utsname.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<dirent.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<signal.h>
#include<ctype.h>

// Colours
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

// Function prototypes
int tokenize(char*, char [], char* []);
void run();
void run_cmd(char*);
void addChildBg(pid_t);
void removeChildBg(int);
void ls(int, char*[]);
void echo(int, char*[]);
void cd(int, char*[]);
void pwd();
void exit_shell();
int check_permissions(int [], char[][5005], char [], char[]);
void pinfo(int, char*[]);
void display_path();
void clear();
void int_to_string(long long int, char[]);
void remindme(int, char*[]);
void clockcmd(int);
void ctrl_c(int);
void ctrl_z(int);
void sigchld(int);
void checkChildren();
void piping(char* [], int);
void setEnv(int, char* []);
void unsetEnv(int, char* []);
void overkill();
void jobs();
void kjob(int, int);
void fg(int);
void bg(int);
char* findStatus(pid_t);
