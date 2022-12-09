# minishell

This is my own Minishell, build at 42 School, and you can find the subject here https://cdn.intra.42.fr/pdf/pdf/22492/fr.subject.pdf .

The goal is to reproduce a basic Bash, with our own builtins (echo, cd, pwd, env, export, unset, exit), commands with relative/absolute path execution, fd redirections (<, >, >>), pipes, signals (ctrl + C, ctrl + D, ctrl + \\), and commands history using termcaps, using UP and DOWN arrows.

Because termcaps effects may vary and do strange things according the OS, the best way to play with this minishell is to run it on a Linux distribution.


