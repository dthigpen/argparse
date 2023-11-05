# Argparse

A Python `argparse` inspired argument parser written for C. This is just meant to be a fun exercise in C, and does not seek to achieve feature parity with the Python implementation.

## Usage

> [!NOTE]
> This project is under active development and does not even have base functionality yet! This section has been provided to show what it might look like.

```c
int main(int argc, char* argv[]) {
    typedef struct Arguments {
        char* path;
        int lines;
    } Arguments;
    Arguments args = {.lines = 10};
    ArgParser parser = {"Description of your program"};
    add_argument(parser, (Arg){"filename", &args.path, .help="The name of the file to open"});
    add_argument(parser, (Arg){"--lines", &args.lines, .transform=&toInt, .help="The max number of lines to read"});
    parse_args(parser);

    printf("Opening file %s and read %d lines..\n",args.path, args.lines);
    return 0;
}
```
