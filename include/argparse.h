#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// char that denotes an optional arg from a positional
static const int ARGPARSE_OPTION_PREFIX_CHAR = '-';

static const char *ARGPARSE_NARGS_OPTIONAL = "?";
static const char *ARGPARSE_NARGS_ONE_OR_MORE = "+";
static const char *ARGPARSE_NARGS_ZERO_OR_MORE = "*";

typedef struct Arg {
    char *name;
    void *dest;
    int (*transform)(char *val, void *dest);
    char *help;
    int required;
    int defaulted; // used to indicate that there is a default value
    char *action;
    char *nargs;
    char *alias;
} Arg;

typedef struct ArgParser {
    char *name;
    char *description;
    Arg *args;
} ArgParser;

static int _check_positional(Arg *arg) {
    // check that they did not also set it as required
    if (arg->required) {
        fprintf(stderr, "'required' is an invalid argument for positionals\n");
        return 1;
    }
    // mark positional arguments as required if at least one is
    // always required
    if (NULL != arg->nargs &&
        0 != strcmp(arg->nargs, ARGPARSE_NARGS_OPTIONAL) &&
        0 != strcmp(arg->nargs, ARGPARSE_NARGS_ZERO_OR_MORE)) {
        arg->required = 1;
    } else if (NULL != arg->nargs &&
               0 == strcmp(arg->nargs, ARGPARSE_NARGS_ZERO_OR_MORE) &&
               !arg->defaulted) {
        arg->required = 1;
    }
    return 0;
}

static int _check_optional(Arg *options) {
    // Skip checking long vs short options
    return 0;
}

int add_argument(ArgParser *parser, Arg arg) {
    // determine if positional or not using the name/alias
    if ((NULL == arg.name && NULL == arg.alias) ||
        (NULL != arg.name && arg.name[0] != ARGPARSE_OPTION_PREFIX_CHAR)) {
        // is positional
        if (_check_positional(&arg)) {
            return 1;
        }
    } else {
        // is optional
        if (_check_optional(&arg)) {
            return 1;
        }
    }

    // handle_action(&arg);
    // if (NULL == parser->args) {
    //     parser->
    // }
    return 0;
}

void _print_args(ArgParser parser) {
    Arg *arg_ptr = NULL;
    for (arg_ptr = parser.args;
         arg_ptr != NULL && (arg_ptr->name != NULL || arg_ptr->alias != NULL);
         arg_ptr += 1) {
        printf("Arg: %s\n", (*arg_ptr).name);
    }
}

void _print_usage(ArgParser parser) {
    // assume max of 1000 chars
    const int line_size = 1000;
    char *buf = (char*)malloc(sizeof(char) * line_size);
    strcpy(buf, "usage: ");
    strcat(buf, parser.name);
    Arg *arg_ptr = NULL;
    for (arg_ptr = parser.args;
         arg_ptr != NULL && (arg_ptr->name != NULL || arg_ptr->alias != NULL);
         arg_ptr += 1) {
        // TODO first print optionals, then positionals
        if (arg_ptr->name != NULL) {
            strcat(buf, " [");
            strcat(buf, arg_ptr->name);
            strcat(buf, "] ");
        }
    }
    buf[line_size - 1] = '\0';
    printf("%s\n\n", buf);

    if (parser.description != NULL) {
        printf("%s\n\n", parser.description);
    };
    free(buf);

    arg_ptr = NULL;
    char *pos_args_buf = malloc(sizeof(char) * 8000);
    char *opt_args_buf = malloc(sizeof(char) * 8000);
    strcpy(pos_args_buf, "");
    strcpy(opt_args_buf, "");
    for (arg_ptr = parser.args;
         arg_ptr != NULL && (arg_ptr->name != NULL || arg_ptr->alias != NULL);
         arg_ptr += 1) {
        char *name = NULL;
        if (arg_ptr->name != NULL) {
            name = arg_ptr->name;
        } else if (arg_ptr->alias != NULL) {
            name = arg_ptr->alias;
        } else {
            printf("Invalid arg, .name or .alias must be set\n");
            continue;
        }
        char *arg_buf = arg_ptr->name[0] == '-' ? opt_args_buf : pos_args_buf;
        char* line_prefix = arg_buf[0] == '\0' ? "  " : "\n  ";
        strcat(arg_buf, line_prefix);
        strcat(arg_buf, arg_ptr->name);
        // TODO offset help message from longest arg name
        if (arg_ptr->help != NULL) {
            strcat(arg_buf, "           ");
            strcat(arg_buf, arg_ptr->help);
        }
    }
    if (strlen(pos_args_buf) > 0) {
        printf("positional arguments:\n%s\n\n", pos_args_buf);
    }
    if (strlen(opt_args_buf) > 0) {
        printf("options:\n%s\n\n", opt_args_buf);
    }
    free(pos_args_buf);
    free(opt_args_buf);
    // sprintf(first_line, "usage: %s %s\n\n%s\n", parser.name, first_line_args,
    // parser.description); if (parser.name != NULL) printf("Name: %s\n",
    // parser.name);
    
}
int parse_args(ArgParser parser, int argc, char **argv) {
    if (parser.name == NULL)
        parser.name = argv[0];
    // print out parser's expected args
    if (parser.args != NULL) {

    } else {
        _print_usage(parser);
        return 1;
    }
    return 0;
}

int toInt(char *val, void *dest) {
    *(int *)dest = atoi(val);
    return 0;
}

int storeTrue(char *val, void *dest) {
    *(int *)dest = 1;
    return 0;
}

int storeFalse(char *val, void *dest) {
    *(int *)dest = 0;
    return 0;
}

int handle_action(Arg *arg) {
    if (NULL == arg->action) {
        return 0;
    }
    if (0 == strcmp(arg->action, "store_true")) {
        arg->transform = &storeTrue;
    } else if (0 == strcmp(arg->action, "store_false")) {
        arg->transform = &storeTrue;
    } else {
        fprintf(stderr, "Unrecognized action: %s\n", arg->action);
        return 1;
    }
    return 0;
}
