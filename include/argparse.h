#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// char that denotes an optional arg from a positional
static const int ARGPARSE_OPTION_PREFIX_CHAR = '-';

static const char *ARGPARSE_NARGS_OPTIONAL = "?";
static const char *ARGPARSE_NARGS_ONE_OR_MORE = "+";
static const char *ARGPARSE_NARGS_ZERO_OR_MORE = "*";
// static const char *ARGPARSE_ACTION_STORE_STR = "store"
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
    char *buf = (char *)malloc(sizeof(char) * line_size);
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
        char *line_prefix = arg_buf[0] == '\0' ? "  " : "\n  ";
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

int defaultTransform(char *val, void *dest) {
    *(char **)dest = val;
    return 0;
}
char *temp_action = "store";
void _parse_optional(char *option_string, char ***option_tuple) {
    if (option_string == NULL || strlen(option_string) == 0) {
        return;
    }
    if (option_string[0] != '-') {
        return;
    }
    // TODO iterate over parser args actions and return matching
    option_tuple[0] = option_string;
    option_tuple[1] = temp_action;
}

int _count_option_strings(Arg *args) {
    Arg *curr_arg = NULL;
    int count = 0;
    for (curr_arg = args; curr_arg->name != NULL || curr_arg->alias != NULL;
         curr_arg += 1) {
        if (curr_arg->name != NULL && curr_arg->name[0] == '-')
            count++;
        if (curr_arg->alias != NULL && curr_arg->alias[0] == '-')
            count++;
    }
    return count;
}

int _count_pos_strings(Arg *args) {
    Arg *curr_arg = NULL;
    int count = 0;
    for (curr_arg = args; curr_arg->name != NULL || curr_arg->alias != NULL;
         curr_arg += 1) {
        if (curr_arg->name != NULL && curr_arg->name[0] != '-')
            count++;
    }
    return count;
}

int parse_args(ArgParser parser, int argc, char **argv) {
    if (parser.name == NULL)
        parser.name = argv[0];

    // Manually check for -h, --help until those get added to args
    printf("argc %d argv[1] %s\n", argc, argv[1]);
    if (argc == 2 &&
        (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        _print_usage(parser);
    } else {
        if (parser.args != NULL) {
            printf("Parsing args..\n");
            // create an array of all optional args and an array of all pos args
            int opt_string_to_arg_map_size = _count_option_strings(parser.args);
            printf("Optional arg+alias count: %d\n",
                   opt_string_to_arg_map_size);
            char **opt_string_to_arg_map_keys =
                malloc(sizeof(char *) * opt_string_to_arg_map_size);
            Arg **opt_string_to_arg_map_values =
                malloc(sizeof(Arg *) * opt_string_to_arg_map_size);
            int opt_string_to_arg_map_index = 0;

            int pos_string_to_arg_map_size = _count_pos_strings(parser.args);
            printf("Pos arg count: %d\n", pos_string_to_arg_map_size);
            char **pos_string_to_arg_map_keys =
                malloc(sizeof(char *) * pos_string_to_arg_map_size);
            Arg **pos_string_to_arg_map_values =
                malloc(sizeof(Arg *) * pos_string_to_arg_map_size);
            int pos_string_to_arg_map_index = 0;
            // loop through each parser arg and map the name and aliases to the
            // Arg ptr
            Arg *curr_arg = NULL;
            Arg *last_non_alias_arg = NULL;
            for (curr_arg = parser.args;
                 curr_arg->name != NULL || curr_arg->alias != NULL;
                 curr_arg += 1) {
                // optional starts with -- and can have aliases
                if (curr_arg->name != NULL && curr_arg->name[0] == '-') {
                    last_non_alias_arg = curr_arg;
                    opt_string_to_arg_map_keys[opt_string_to_arg_map_index] =
                        curr_arg->name;
                    opt_string_to_arg_map_values[opt_string_to_arg_map_index] =
                        last_non_alias_arg;
                } else if (curr_arg->alias != NULL &&
                           curr_arg->alias[0] == '-') {
                    opt_string_to_arg_map_keys[opt_string_to_arg_map_index] =
                        curr_arg->alias;
                    opt_string_to_arg_map_values[opt_string_to_arg_map_index] =
                        last_non_alias_arg;
                } else if (curr_arg->name != NULL && curr_arg->name[0] != '-') {
                    // pos arg
                    pos_string_to_arg_map_keys[pos_string_to_arg_map_index] =
                        curr_arg->name;
                    pos_string_to_arg_map_values[pos_string_to_arg_map_index] =
                        curr_arg;
                }
            }
            // Now we have a mapping from all parser arg strings to their Arg
            // data structure option_string_indices map key: index of actual
            // argv string value: associated Arg that it matches to if all args
            // are optional then argc -1 is max array size
            int option_string_indices_map_size = argc;
            int option_string_indices_map_keys =
                malloc(sizeof(int) * option_string_indices_map_size);
            Arg *option_string_indices_map_keys =
                malloc(sizeof(Arg *) * option_string_indices_map_size);
            char *arg_string_pattern_parts = malloc(sizeof(char) * argc);
            for (int i = 1; i < argc; i++) {
                arg_string_pattern_parts[i] = '_';
                option_string_indices_map_keys[i] = -1;
                option_string_indices_map_values[i] = NULL;

                if (strcmp(argv[i], "--")) {
                    arg_string_pattern_parts[i] = '-';
                    for (int j = i + 1; j < argc; j++) {
                        arg_string_pattern_parts[j] = 'A';
                    }
                } else {
                    if(argv[i][0] != '-') {
                        arg_string_pattern_parts[i] = 'A';
                    } else {
                        arg_string_pattern_parts[i] = 'O';
                        // TODO find pattern and put it in map
                        option_string_indices_map_values[i] = NULL;
                    }
                }
            }

            // get positionals
            Arg **positionals =
                malloc(sizeof(Arg *) * pos_string_to_arg_map_size);
            for (int i = 0; i < pos_string_to_arg_map_size; i++) {
                positionals[i] = pos_string_to_arg_map_values[i];
            }
            char **extras = malloc(sizeof(char *) * argc);
            int start_index = 0;
            // max_option_string_index is -1 if no optional args e.i only
            // positionals otherwise
            //         char *arg_string_pattern_parts =
            //             malloc(sizeof(char) * argc);
            // char ****option_string_indices = malloc(sizeof(char ***) *
            // argc);
            // // char*** option_string_indices[] = {{"hi there",
            // "bye"},NULL}; for (int i = 1; i < argc; i++) {
            //     arg_string_pattern_parts[i] = '_';
            //     option_string_indices[i] = NULL;

            //     if (strcmp(argv[i], "--")) {
            //         arg_string_pattern_parts[i] =
            //             '-' for (int j = i + 1; j < argc; j++) {
            //             arg_string_pattern_parts[j] = 'A';
            //         }
            //     } else {
            //         char *option_tuple[2] = {NULL, NULL};
            //     }
            // }
            //-------------------------
            // create extras array for at max all args
            // char **extras = malloc(sizeof(char *) * argc);
            // int start_index = 0;
            // int argv_index = 1;
            // Arg *curr_arg = NULL;
            // Arg *last_non_alias_arg = NULL;
            // for (curr_arg = parser.args;
            //      curr_arg->name != NULL || curr_arg->alias != NULL;
            //      curr_arg += 1) {
            //     if (curr_arg->name != NULL) {
            //         last_non_alias_arg = curr_arg;
            //     }
            //     const char *action = "store";
            //     int (*transform)(char *val, void *dest) = &defaultTransform;

            //     if (last_non_alias_arg->action != NULL) {
            //         action = last_non_alias_arg->action;
            //     }
            //     if (last_non_alias_arg->transform != NULL) {
            //         transform = last_non_alias_arg->transform;
            //     }
            //     printf("here\n");
            //     if(0 == strcmp(action, "store")) {
            //         transform(argv[argv_index], last_non_alias_arg->dest);
            //     }
            //     argv_index++;
            // }
        }
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
