# peek

`peek` is a simple yet powerful command-line utility to explore and inspect directories. It's inspired by tools like `ls`, but with added functionality and more readable output. It supports features like recursive tree views, grouped file listings, timestamped views, and more.

## Features

- List files and folders with a clean, readable layout
- Group files by type and extension
- Show last modified timestamps
- Display a directory tree recursively
- Automatically colorize output by file type

## Installation

To install `peek` globally on your system:

1. Clone this repository:

```bash
git clone https://github.com/yourusername/peek.git
cd peek
./install.sh
```
This will compile the peek binary and place it in your system's executable path (e.g., /usr/local/bin), allowing you to run peek from anywhere in the terminal.

## Usage
```bash
peek [option] [path]
```

## Options
Flag	Description
-g	Group files by type and extension
-t	Show last modified timestamps
-tr	Display a recursive directory tree
-h	Show usage help

## Dependencies
GCC or any C compiler
Bash (or any POSIX-compliant shell)
Make (optional, if you add a Makefile later)

## Compatibility
peek works on all major Linux distributions. The install script uses /usr/bin/env bash for maximum compatibility across different shell environments.

## Contributing
Pull requests are welcome! If you have ideas for improvements or additional features, feel free to fork the repo and open a PR.
Whether it’s a bug fix, new flag, improved formatting, or just a cleanup — all contributions are appreciated.

## Support the Project
If you found peek useful, consider giving this repo a ⭐️ to help others discover it. It helps a lot!
