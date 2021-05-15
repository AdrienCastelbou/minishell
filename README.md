# minishell

This is my own Minishell, build at 42 School, and you can find the subject here https://cdn.intra.42.fr/pdf/pdf/22492/fr.subject.pdf
The goal is to reproduce a basic Bash, with our own builtins (echo, cd, pwd, env, export, unset, exit), commands with relative/absolute path execution, fd redirections (<, >, >>), pipes, signals (ctrl + C, ctrl + D, ctrl + \), and commands history using termcaps, using UP and DOWN arrows.
Because termcpas effects may vary and do strange things according to the env, the best way to play with this minishell is using the 42 Virtual Machine, which works with Ubuntu. You can find it here : https://cdn.42.fr/xubuntu-42.ova .


