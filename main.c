// main.c
#include "argv.h"
#include "builtins.h"
#include "helpers.h"
#include "parse.h"
#include "path.h"
#include "redirection.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void
run_command(char *cmd)
{
    char *outfile = NULL;
    int redir = detect_redirection(cmd, &outfile);
    if(redir == -1)
	{
	    print_error();
	    return;
	}

    int argc;
    char **argv = build_argv(cmd, &argc);
    if(argc == 0)
	{
	    free(argv);
	    if(outfile)
		free(outfile);
	    return;
	}

    Builtin *b = builtin_lookup(argv[0]);
    if(b)
	{
	    b->fn(argc, argv);
	}
    else
	{
	    // not a builtin, find in PATH
	    char *execpath = find_executable(argv[0]);
	    if(!execpath)
		{
		    print_error();
		}
	    else
		{
		    pid_t pid = fork();
		    if(pid < 0)
			{
			    print_error();
			}
		    else if(pid == 0)
			{
			    // in child, do redirection then exec
			    if(redir == 1)
				{
				    int fd = open(outfile,
						  O_CREAT | O_WRONLY | O_TRUNC,
						  S_IRWXU);
				    if(fd < 0)
					{
					    print_error();
					    exit(1);
					}
				    dup2(fd, STDOUT_FILENO);
				    dup2(fd, STDERR_FILENO);
				    close(fd);
				}
			    execv(execpath, argv);
			    print_error();
			    exit(1);
			}
		    else
			{
			    waitpid(pid, NULL, 0);
			}
		    free(execpath);
		}
	}

    for(int i = 0; i < argc; i++)
	free(argv[i]);
    free(argv);
    if(outfile)
	free(outfile);
}

int
main(int argc, char *argv[])
{
    FILE *input = stdin;
    int interactive = 1;

    if(argc == 1)
	{
	    interactive = 1;
	}
    else if(argc == 2)
	{
	    interactive = 0;
	    input = fopen(argv[1], "r");
	    if(!input)
		{
		    print_error();
		    exit(1);
		}
	}
    else
	{
	    print_error();
	    exit(1);
	}

    path_init();

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while(1)
	{
	    if(interactive)
		{
		    printf("kreash> ");
		    fflush(stdout);
		}
	    linelen = getline(&line, &linecap, input);
	    if(linelen == -1)
		{
		    free(line);
		    exit(0);
		}

	    trim(line);
	    if(strlen(line) == 0)
		continue;

	    // break on ; for sequential execution
	    int nseq;
	    char **seq = split_char(line, ';', &nseq);
	    for(int i = 0; i < nseq; i++)
		{
		    if(strlen(seq[i]) == 0)
			{
			    free(seq[i]);
			    continue;
			}

		    // break on & for parallel execution
		    int npar;
		    char **par = split_char(seq[i], '&', &npar);
		    pid_t *pids = malloc(npar * sizeof(pid_t));

		    for(int j = 0; j < npar; j++)
			{
			    if(strlen(par[j]) == 0)
				{
				    free(par[j]);
				    continue;
				}
			    pid_t pid = fork();
			    if(pid == 0)
				{
				    run_command(par[j]);
				    exit(0);
				}
			    else
				{
				    pids[j] = pid;
				}
			    free(par[j]);
			}

		    for(int j = 0; j < npar; j++)
			{
			    if(pids[j] > 0)
				waitpid(pids[j], NULL, 0);
			}
		    free(pids);
		    free(par);
		    free(seq[i]);
		}
	    free(seq);
	}
}
