# Linux Shell

## How to use?
- To complile the program, run `make`
- To execute the executable thus created, run `./shell`
- use the `quit` command to exit the shell
- You can run `make clean` on your terminal to remove executables generated by make

## Commands:
**Builtins:**
- `ls`
  - `ls -l`
  - `ls -a`
  - `ls -la`
  - `ls -al`
- `echo`
- `cd`
  - `cd <dir_path>`
- `pwd`  
- `pinfo`
  - `pinfo <pid>`
- `clear`
- `clock -t <interval>`
- `remindme <duration> <message>`
- `quit`
- `setenv <var> <value>`
- `unsetenv <var>`
- `overkill`
- `jobs`
- `kjob <jobNumber> <signalNumber>`
- `fg <jobNumber>`
- `bg <jobNumber>`

## Other features:
- Redirection operators:
  - `>` output redirection (overwrite)
  - `>>` output redirection (append)
  - `<` input redirection
- `|` Piping operator
- **Note:** *The piping operator can be used in combination with the redirection operators*
- `Ctrl + C` kills the current foreground proccess.
- `Ctrl + Z` sends a foreground process to background and changes it's state to ***Stopped***.
- All other terminal commands are excuted using fork.
- Processes can be run in foreground or background in this C - shell. To run in background, simply add an '&' to the end of a command. <br/>For example: `gedit &` will open *gedit* in the background, which allows us to continue using the shell.
- The user will be notified when a background process ends (including exit status).
