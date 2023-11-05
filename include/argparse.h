#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// char that denotes an optional arg from a positional
static const int ARGPARSE_OPTION_PREFIX_CHAR = '-';

static const char *ARGPARSE_NARGS_OPTIONAL = "?";
static const char *ARGPARSE_NARGS_ONE_OR_MORE = "+";
static const char *ARGPARSE_NARGS_ZERO_OR_MORE = "*";

typedef struct ArgParser {
    char *description;
} ArgParser;

typedef struct Arg {
    char *name;
    void *dest;
    int (*transform)(char *val, void *dest);
    char *help;
    int required;
    int defaulted; // used to indicate that there is a default value
    char *action;
    char *nargs;
    char **aliases;
} Arg;


int add_argument(ArgParser parser, Arg options);


int toInt(char *val, void *dest)
{
    *(int *)dest = atoi(val);
    return 0;
}


int parse_args(ArgParser parser);
