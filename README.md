# dsh

a shell for learning. has exit, cd, and path builtins. supports running commands sequentially (;), in parallel (&), and redirecting output (>).

## build and run

```sh
make
./dsh
```

## interactive mode

```sh
./dsh
```

you get a prompt `dsh> `. type commands. exit with the `exit` command or press ctrl+d.

## batch mode

```sh
./dsh script.sh
```

reads all commands from the script file and runs them. no prompt. exits when done or if there's an error.

## builtins

these three commands run inside the shell itself, not as external programs.

### exit

```sh
exit
```

exits the shell. takes no arguments.

### cd

```sh
cd /home/user
```

changes the current directory. if the directory doesn't exist or you don't have permission, it fails silently and the shell keeps running.

### path

```sh
path /bin /usr/bin /usr/local/bin
```

sets where the shell looks for executable programs. you list all the directories you want. this completely replaces the previous path, it doesn't add to it.

```sh
path
```

clears the path. now external commands won't run at all because there's nowhere to look for them. you can set it again with another `path` command.

the path you set stays for the whole session. it's not global—it only affects this shell instance.

## running commands

the shell can execute programs found in your `path`. use the standard unix conventions.

```sh
ls -l
grep pattern file.txt
./program arg1 arg2
```

if the program is not found in any directory in your path, the shell prints an error.

## execution modes

### sequential

```sh
cmd1 ; cmd2 ; cmd3
```

runs cmd1, waits for it to finish, then runs cmd2, waits, then runs cmd3. each command runs one after the other.

### parallel

```sh
cmd1 & cmd2 & cmd3
```

starts cmd1, cmd2, and cmd3 all at once. the shell waits for all of them to finish before continuing. useful for running multiple slow things together.

### output redirection

```sh
ls > output.txt
```

sends the output (stdout and stderr) of a command to a file instead of printing it to the terminal. the file is created or truncated if it already exists.

### combining them

```sh
ls & sleep 2 ; cat file.txt > out.txt
```

this runs `ls` and `sleep 2` in parallel, waits for both to finish, then runs `cat file.txt` and writes the output to `out.txt`.

```sh
cmd1 > file1.txt & cmd2 > file2.txt
```

runs two commands in parallel, each writing to a different file.

## adding new builtins

all builtins are defined in `builtins.c`. there's an array called `builtins[]` that lists them all.

to add a new builtin:

1. write a function in `builtins.c`:

```c
int builtin_hello(int argc, char **argv)
{
    printf("hello world\n");
    return 0;
}
```

the function gets the command line arguments in `argc` and `argv`. return 0 on success, 1 on error. return value doesn't really do anything right now but follow the convention.

2. add it to the `builtins[]` array in `builtins.c`:

```c
static Builtin builtins[] = {
    {"exit", builtin_exit},
    {"cd", builtin_cd},
    {"path", builtin_path},
    {"hello", builtin_hello},
    {NULL, NULL}
};
```

that's all. the `builtin_lookup` function finds it by searching the array. `main.c` doesn't need to change at all.

## source code

- `main.c` - reads input, splits on `;` and `&`, forks processes, waits for them
- `builtins.c` - all three builtin commands, the lookup table, the lookup function
- `builtins.h` - function pointer type, declarations
- `argv.c` - takes a string and splits it into argc/argv
- `parse.c` - splits a string on a character (used for `;` and `&`)
- `path.c` - stores the search path, finds executables in those directories
- `redirection.c` - detects `>` in a command and extracts the filename
- `helpers.c` - trim whitespace, print error messages

## how the shell works

when you type a command, the shell checks if it's a builtin. if it is, the builtin function runs right there in the parent process. if not, the shell forks a child process, looks for the executable in your `path`, and runs it. external commands run in children so the parent can keep going.

for `;`, the shell waits for each command to finish before starting the next one.

for `&`, the shell forks all the commands at once, then waits for all of them to finish.

redirection happens in the child process right before execve, so the file handling doesn't affect the parent.

builtins have to run in the parent because cd and path and exit need to affect the shell itself. if they ran in a child, the changes would be lost when the child exited.

## errors

when something goes wrong, the shell prints "An error has occurred\n" to stderr. in interactive mode it keeps running. in batch mode it exits immediately.

errors happen for things like:
- executable not found in path
- permission denied
- can't open a redirection file
- cd to a directory that doesn't exist
- wrong number of arguments to a builtin
