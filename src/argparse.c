#include "../include/argparse.h";
#include <assert.h>

static int check_positional(Arg *arg) {
    // check that they did not also set it as required
    if (arg->required) {
        fprintf(stderr, "'required' is an invalid argument for positionals\n");
        return 1;
    }
    // mark positional arguments as required if at least one is
    // always required
    if (0 != strcmp(arg->nargs, ARGPARSE_NARGS_OPTIONAL) &&
        0 != strcmp(arg->nargs, ARGPARSE_NARGS_ZERO_OR_MORE)) {
        arg->required = 1;
    } else if (0 == strcmp(arg->nargs, ARGPARSE_NARGS_ZERO_OR_MORE) &&
               !arg->defaulted) {
        arg->required = 1;
    }
    arg->aliases = NULL;
    return 0;
}

static int check_optional(Arg *options) {

    // Skip checking long vs short options
    return 0;
}

int add_argument(ArgParser parser, Arg arg) {
    // determine if positional or not using the name/aliases
    if ((NULL == arg.name && NULL == arg.aliases) ||
        (NULL != arg.name && arg.name[0] != ARGPARSE_OPTION_PREFIX_CHAR)) {
        // is positional
        if (check_positional(&arg)) {
            return 1;
        }
    } else {
        // is optional
        if (check_optional(&arg)) {
            return 1;
        }
    }

    // create the action object, and add it to the parser
    // TODO handle actions
}

int parse_args(ArgParser parser) {
    fprintf(stderr, "Not implemented\n");
    return 1;
}

int main(void) {
    typedef struct ProgArgs {
        char *path;
        int lines;
    } ProgArgs;
    ProgArgs args = {.lines = 10};
    ArgParser parser = {"This is a program description"};
    add_argument(parser, (Arg){"filename", &args.path,
                               .help = "The name of the file to open"});
    add_argument(parser, (Arg){"--lines", &args.lines, .transform = &toInt,
                               .help = "The max number of lines to read"});
    parse_args(parser);
    printf("Read %d lines from file %s\n", args.lines, args.path);
    return 0;
}
