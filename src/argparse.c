#include "../include/argparse.h"
#include <assert.h>

int main(int argc, char *argv[]) {
    typedef struct ProgArgs {
        char *path;
        int lines;
    } ProgArgs;
    ProgArgs args = {.lines = 10};
    ArgParser parser = {
        .name = "my-prog",
        .description = "This is a program description",
        (Arg[]){
            {"filename", &args.path, .help = "The name of the file to open"},
            {"pos_arg2", &args.path},
            {"--lines", &args.lines, .transform = &toInt,
             .help = "The max number of lines to read"},
            {0}},
    };
    _print_usage(parser);
    // parse_args(parser, argc, argv);
    // printf("Read %d lines from file %s\n", args.lines, args.path);
    return 0;
}
