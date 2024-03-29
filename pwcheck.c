#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LENGTH 100  // reserve one character for termination.
#define BUFFER_SIZE MAX_LENGTH + 1

struct stats_t {
    bool unique_chars[256];

    int min;

    float sum;
    float pass_count;
};

typedef struct stats_t Stats;

/*
    determine the length of the string @str by the position of the terminating
    null byte.
*/
int length(const char *str) {
    int n;
    for (n = 0; str[n] != '\0'; n++)
        ;
    return n;
}

/*
    compare two strings
*/
bool cmp(const char *str1, const char *str2) {
    for (int i = 0; str1[i] != '\0' || str2[i] != '\0'; i++) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }

    return true;
}

bool find(const char *where, const char *what, int start) {
    int n = 0;  // index for @what

    for (int i = start; where[i] != '\0'; i++) {
        if (what[n] == where[i]) {
            n++;

            // reached the end (aka found whole string)
            if (what[n] == '\0') {
                return true;
            }
        } else {
            n = 0;  // reset @what index to 0
        }
    }

    return false;
}

// check whether a string contains at least one character the an ASCII range
// specified by low & high
bool ascii_range(const char *str, char low, char high) {
    for (int n = 0; str[n] != '\0'; n++) {
        if (str[n] >= low && str[n] <= high) {
            return true;
        }
    }

    return false;
}

// check whether a string contains any special ASCII characters
int special_chars(const char *str) {
    return (ascii_range(str, ' ', '/') || ascii_range(str, ':', '@') ||
            ascii_range(str, '[', '`') || ascii_range(str, '{', '~'));
}

// count how many duplicate sequential characters there are in a given string,
// until a threshold is reached level = 3 param = 3 // max. počet stejných
// charakterů za sebou
bool duplicate_chars(const char *str, int threshold) {
    char last = str[0];
    int count = 1;

    for (int i = 1; str[i] != '\0'; i++) {
        if (str[i] == last) {
            count += 1;
        } else {
            count = 1;
        }

        last = str[i];

        if (count >= threshold) {
            return true;
        }
    }

    return false;
}

// slice a string, return into @param buff
void slice(const char *str, char *buff, int start, int end) {
    for (int n = start; str[n] != '\0' && n < end; n++) {
        buff[n - start] = str[n];
    }

    buff[end - start] = '\0';  // terminate
}

// check whether a string contains any duplicate substrings of given length
bool duplicate_substrings(const char *str, int sub_length) {
    int len = length(str);

    char buff[sub_length];

    for (int n = 0; n < len - sub_length + 1; n++) {
        slice(str, buff, n, n + sub_length);

        if (find(str, buff, n + 1)) {
            return true;
        }
    }

    return false;
}

// parse a long from given string @str, save into @res
bool parse_long(const char *str, long *res) {
    char *ptr;
    long extractedValue = strtol(str, &ptr, 10);

    if (ptr[0] != '\0') {
        return false;
    }

    *res = extractedValue;
    return true;
}

// parse a boolean switch from @arg, save into @res
bool parse_bool_switch(const char *arg, char *param, char *alias, bool *res) {
    if (cmp(arg, param) || cmp(arg, alias)) {
        *res = true;
        return true;
    }

    return false;
}

/*
    attempt to extract a long parameter from @arg and it's value from @next_arg,
 */
bool parse_long_param(const char *arg, const char *next_arg, const char *param,
                      const char *alias, long *res, int *sign) {
    if (cmp(arg, param) || cmp(arg, alias)) {
        if (next_arg == NULL) {
            *sign = 2;
            return true;
        }

        if (!parse_long(next_arg, res)) {
            *sign = 2;
            return true;
        }

        *sign = 1;
        return true;
    }

    return false;
}

// parse a fixed long param from @arg, save into @value
bool parse_fixed(const char *arg, long *value, bool switches) {
    if (!parse_long(arg, value)) {
        fprintf(stderr, "Level has to be a number.\n");
        return false;
    }

    if (switches) {
        fprintf(stderr, "Invalid arguments.\n");
        return false;
    }

    return true;
}

/*
    attempt to extract @level & @param from given command line arguments @argv
   (of length @argc), save the results into @level, @param, @stats
 */
bool extract_params(int argc, char const *argv[], long *level, long *param,
                    bool *stats) {
    int level_sign = 0;
    int param_sign = 0;

    bool switches = false;  // an argument matched the switch -l 2 -p 2 syntax.
    bool fixed = false;     // an argument matched the fixed LEVEL PARAM syntax.

    for (int n = 1; n < argc; n++) {
        const char *arg = argv[n];
        const char *nextArg = n + 1 < argc ? argv[n + 1] : NULL;

        // attempt to extract any switches with long values
        if (!fixed && (parse_long_param(arg, nextArg, "--level", "-l", level,
                                        &level_sign) ||
                       parse_long_param(arg, nextArg, "--param", "-p", param,
                                        &param_sign))) {
            n++;  // skip the next argument, that's the value of the switch we
            // just parsed.

            switches = true;
        } else if (parse_bool_switch(arg, "--stats", "-s", stats)) {
            // the arguments is a switch
        } else {
            // look for level & param values in the argument

            fixed = true;

            if (level_sign == 0) {
                if (!parse_fixed(argv[n], level, switches)) {
                    fprintf(stderr, "Invalid arguments.\n");
                    return false;
                }

                level_sign = 1;
            } else if (param_sign == 0) {
                if (!parse_fixed(argv[n], param, switches)) {
                    fprintf(stderr, "Invalid arguments.\n");
                    return false;
                }

                param_sign = 1;
            } else {
                fprintf(stderr, "Too many arguments!\n");
                return false;
            }
        }

        // arguments entered are invalid
        if (level_sign == 2 || param_sign == 2) {
            fprintf(stderr, "Invalid argument %s.\n", argv[n]);
            return false;
        }
    }

    if (fixed && param_sign == 0) {
        fprintf(stderr, "Not enough arguments.\n");
        return false;
    }

    return true;
}

// check the bounds of submitted arguments
bool check_bounds(long level, long param) {
    if (level < 1) {
        fprintf(stderr, "Level too low. Allowed: <1; 4>\n");
        return false;
    }

    if (level > 4) {
        fprintf(stderr, "Level too high. Allowed: <1; 4>\n");
        return false;
    }

    // param

    if (param < 1) {
        fprintf(stderr, "Invalid param. Has to be a positive number.\n");
        return false;
    }

    return true;
}

// calculate the score of a password based on how many char groups it contains
long calculate_score(const char *str) {
    return ascii_range(str, 'A', 'Z') +  // uppercase alfanumeric
           ascii_range(str, 'a', 'z') +  // lowercase alfanumeric
           ascii_range(str, '0', '9') +  // numbers
           special_chars(str);           // special characters
}

bool check_levels(const char *pass, long level, long param) {
    // Check the levels.

    if (level >= 1 &&
        (!ascii_range(pass, 'A', 'Z') || !ascii_range(pass, 'a', 'z'))) {
        return false;
    }

    if (level >= 2 &&
        calculate_score(pass) < (param > 4 ? 4 : param)) {  // cap param at 4
        return false;
    }

    if (level >= 3 && duplicate_chars(pass, param)) {
        return false;
    }

    if (level >= 4 && duplicate_substrings(pass, param)) {
        return false;
    }

    return true;
}

// read a password from stdin
bool read_pass(char *pass, Stats *stats, int *length) {
    int c;  // currently read char
    int i;  // index

    for (i = 0;; i++) {
        c = fgetc(stdin);

        if (i > MAX_LENGTH) {
            // just return a number higher than limit to throw an err
            *length = MAX_LENGTH + 1;
            return false;
        }

        if (c == EOF) {
            return false;
        }

        if (c == '\n') {
            break;
        }

        // count into unique chars
        if (!stats->unique_chars[c]) {
            stats->unique_chars[c] = true;
        }

        pass[i] = c;
    }

    *length = i;
    pass[i] = '\0';  // terminate the password string with a null byte

    return true;
}

// process after-load statistics
void process_stats(Stats *statistics, int length) {
    // check whether the password is shorter than the current shortest one
    if (length < statistics->min || statistics->min == 0) {
        statistics->min = length;
    }

    statistics->pass_count++;
    statistics->sum += length;
}

int main(int argc, const char *argv[]) {
    long level = 1;
    long param = 1;
    bool show_stats = false;

    // extract values from given arguments

    if (!extract_params(argc, argv, &level, &param, &show_stats)) {
        return EXIT_FAILURE;
    }

    // check the bounds of extracted arguments

    if (!check_bounds(level, param)) {
        return EXIT_FAILURE;
    }

    // Setup stats

    Stats stats = {{false}, 0, 0, 0};

    // Read & process passwords

    char pass[BUFFER_SIZE];
    int length = 0;  // length of the password

    while (read_pass(pass, &stats, &length)) {
        process_stats(&stats, length);

        // Print when we pass all of the required levels.
        if (check_levels(pass, level, param)) {
            printf("%s\n", pass);
        }
    }

    if (length > MAX_LENGTH) {
        fprintf(stderr, "Password too long.\n");
        return EXIT_FAILURE;
    }

    // Print stats if requested.

    if (show_stats) {
        // calculate unique characters

        int unique = 0;

        for (int i = 0; i < 128; i++) {
            if (stats.unique_chars[i]) {
                unique++;
            }
        }

        printf(
            "Statistika:\nRuznych znaku: %d\nMinimalni delka: "
            "%d\nPrumerna "
            "delka: %.1f\n",
            unique, stats.min, (stats.sum / stats.pass_count));
    }

    return 0;
}