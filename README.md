# Microshell
## Created by Michał Hołubowicz
##### mharcerz@gmail.com

Microshell is a shell project for linux users, implemented in 2021 by me in C
language as a semester project from the Operating Systems course.

## Features

- Command support:
    - cd
    - exit
    - help
    - pwd
    - cut (-c,-f)
    - cal
- Forwarding unsupported commands to other programs for execute
- Color support
- Displaying the login of the currently logged in user
- Command history handler
- Signal handling
- File and directory syntax completion
- Support for arguments in quotation marks, separated by spaces, tabs


## Installation

##### To execute Microshell we need download two libraries
So, open your bash and:


- First is <ncurses.h>

```sh
sudo apt-get install libncurses5-dev
```

- Second are <readline/readline.h> and <readline/history.h>

```sh
sudo apt-get install libreadline-dev
```

## Execute

Download project from git:

```sh
git clone https://github.com/mharcerz/microshell.git
cd microshell
```

Now, we can execute Microshell:

```sh
gcc microshell.c -lreadline -o microshell
./miroshell
```

Now, you can execute help to find out what microshell has to offer:
```sh
help
```

Enjoy!

