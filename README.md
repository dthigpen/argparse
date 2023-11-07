# Argparse

A Python `argparse` inspired argument parser written for C. This is just meant to be a fun exercise in C, and does not seek to achieve feature parity with the Python implementation.

## Usage

> [!NOTE]
> This project is under active development and does not even have base functionality yet! This section has been provided to show what it might look like.

```c
int main(int argc, char* argv[]) {
    // parsed args to be used throughout program
    typedef struct Arguments {
        char* path;
        int lines;
    } Arguments;
    // set default values first
    Arguments args = {.lines = 10};
    ArgParser parser = {"Description of your program"};
    Arg* options = {
      {"filename", &args.path, .help="The name of the file to open"},
      {"--lines", &args.lines, .transform=&toInt, .help="The max number of lines to read"},
      {0}};
    // parse into Arguments struct
    parse_args(parser, options, argc, argv);

    printf("Opening file %s and read %d lines..\n",args.path, args.lines);
    return 0;
}
```
