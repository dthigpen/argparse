# Argparse

A Python `argparse` inspired argument parser written for C. This is just meant to be a fun exercise in C, and does not seek to achieve feature parity with the Python implementation.

## Usage

> [!NOTE]
> This project is under active development and does not even have base functionality yet! This section has been provided to show what it might look like.

```c
int main(int argc, char** argv) {
    // my program arguments struct to be populated
    typedef struct {
        char* path;
        int lines;
    } Arguments;
    // create instance and set defaults beforehand
    Arguments args = {.lines = 10};
    // create parser
    ArgParser parser = {
        .description = "Description of your program",
        .args = (Arg[]){{"filename", &args.path, .help = "The name of the file to open"},
                        {"--lines", &args.lines, .transform = &toInt, .help = "The max number of lines to read"},
                        {0}},
    };
    parse_args(parser);
    printf("Opening file %s and read %d lines..\n",args.path, args.lines);
    return 0;
}
```
