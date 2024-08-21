
# Simple Shell Project

This Simple Shell project is a minimalistic command-line interpreter that mimics some of the basic functionalities of a Unix shell. It allows execution of shell commands, changing directories, printing working directories, and maintains a history of executed commands. It is implemented in C and demonstrates the use of system calls, process handling, and environment variable management.

## Features

- Execute common Linux commands.
- Change the current working directory with `cd`.
- Print the current working directory with `pwd`.
- Maintain a history of executed commands.
- Dynamic environment variable path manipulation.

## Installation

Clone the repository to your local machine using:

```bash
git clone https://github.com/yourusername/simple-shell.git
```

Navigate to the cloned directory:

```bash
cd simple-shell
```

## Usage

Compile the project using:

```bash
gcc -o simple_shell main.c
```

Run the shell:

```bash
./simple_shell
```

Optionally, you can add directories to the PATH environment variable when launching the shell:

```bash
./simple_shell /usr/local/bin /another/path
```

## How it Works

The shell operates by continuously reading user input, parsing it, and executing commands until the `exit` command is given. It supports adding extra paths to the system PATH variable at runtime, allowing for flexible command execution environments.

### Command History

Commands are stored in a circular queue, allowing for a finite history that maintains the most recent commands up to a limit, defined by `MAX_STRINGS`.

### Environment Variables

Environment paths can be dynamically added at shell startup, allowing for customized command searches without altering the global system environment permanently.
