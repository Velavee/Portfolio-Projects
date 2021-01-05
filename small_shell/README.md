
# About
This program is a small shell in the style of Linux/Unix. The commands 'exit', 'cd', and 'status' are built into the shell, while other commands are handled by new processes and exec functions. The shell allows input and output redirection as well as foreground and background processes. There are two custom signal handlers: one for SIGINT and one for SIGTSTP.

# Operating Instructions
To compile in Linux, use the following command:

$ gcc -std=gnu99 -g -Wall -o smallsh smallsh.c

To execute the program, use the following command:

$ ./smallsh
