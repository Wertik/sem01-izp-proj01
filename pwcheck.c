#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int length(char *str) {
    int n;
    for (n = 0; str[n] != '\0'; n++) {}
    return n;
}

int cmp(const char *str1, const char *str2) {
    for (int i = 0; str1[i] != '\0' || str2[i] != '\0'; i++) {
        if (str1[i] != str2[i]) {
            return 0;
        }
    }

    return 1;
}

int containsc(char *where, char what) {
    for (int i = 0; where[i] != '\0'; i++) {
        if (what == where[i]) {
            return 1;
        }
    }

    return 0;
}

// find a string in another
// return the count of findings
int find(char *where, char *what) {
    int n = 0;

    int count = 0;

    for (int i = 0; where[i] != '\0'; i++) {
        if (what[n] == where[i]) {
            n++;

            // reached the end (aka found whole string)
            if (what[n] == '\0') {
                count += 1;

                i -= n - 1; // move back to the start of the matched string
                n = 0;
            }
        } else {
            n = 0; // reset to 0
        }
    }

    return count;
}

// check whether a string contains at least one character the an ASCII range specified by low & high
int containsASCIIRange(char *str, char low, char high) {
    for (int n = 0; str[n] != '\0'; n++) {
        if (str[n] >= low && str[n] <= high) {
            return 1;
        }
        }

    return 0;
}

// check whether a string contains any special ASCII characters
int containsSpecialCharacters(char *str) {
    return (containsASCIIRange(str, ' ', '/') ||
            containsASCIIRange(str, ';', '@') ||
            containsASCIIRange(str, '[', '`') ||
            containsASCIIRange(str, '(', '~'));
}

// count how many duplicate sequential characters there are in a given string, until a threshold is reached
// level = 3
// param = 3 // max. počet stejných charakterů za sebou
int checkDuplicateChars(char *str, int threshold) {
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
            return 1;
        }
    }

    return 0;
}

// slice a string, return into @param buff
void slice(char *str, char *buff, int start, int end) {
    for (int n = start; str[n] != '\0' && n < end; n++) {
        buff[n - start] = str[n];
    }

    buff[end - start] = '\0'; // terminate
}

// check whether a string contains any duplicate substrings of given length
int containsDuplicateSubstrings(char *str, int subLength) {
    int len = length(str);

    // hold the substring we're looking for
    char buff[subLength];

    for (int n = 0; n < len - subLength + 1; n++) {
        slice(str, buff, n, n + subLength);

        // found more than the original
        if (find(str, buff) > 1) {
            return 1; // return true
        }
    }

    return 0;
}

int extractBoolSwitch(const char *arg, char *param, char *alias, bool *res) {
    if (cmp(arg, param) || cmp(arg, alias)) {
        *res = true;
        return 1;
    }

    return 0;
}

int parseLong(const char *str, long *res) {
    char *ptr;
    long extractedValue = strtol(str, &ptr, 10);

    if (ptr[0] != '\0') {
        return 0;
    }

    *res = extractedValue;
    return 1;
}

int extractLongParam(const char *arg, const char *nextArg, char *param, char *alias, long *res, int *sign) {

    if (cmp(arg, param) || cmp(arg, alias)) {

        if (nextArg == NULL) {
            fprintf(stderr, "No value for %s provided.", param);
            *sign = 2;
            return 1;
        }

        if (!parseLong(nextArg, res)) {
            fprintf(stderr, "%s value has to be a number. Provided: %s\n", arg, nextArg);
            *sign = 2;
            return 1;
        }

        *sign = 1;
        return 1;
    }

    return 0;
}

int containsInt(int *arr, int len, int lf) {
    for (int n = 0; n < len; n++) {
        if (arr[n] == lf) {
            return 1;
        }
    }

    return 0;
}

int extractParams(int argc, char const *argv[], long *level, long *param, bool *stats) {
    int levelSign = 0;
    int paramSign = 0;

    // Support both formats

    // pwcheck LEVEL PARAM --stats
    // pwcheck 4 2 --stats

    // pwcheck -l 4 -p 2 --stats

    // pwcheck 4 -p 2 --stats

    // filter out switch arguments, then use the rest to try and find level & param

    // Note: When you use both (argument & switch) to define a value, whichever comes last overrides any values defined before.

    for (int n = 1; n < argc; n++) {
        const char *arg = argv[n];
        const char *nextArg = n + 1 < argc ? argv[n + 1] : NULL;

        if (extractLongParam(arg, nextArg, "--level", "-l", level, &levelSign) ||
            extractLongParam(arg, nextArg, "--param", "-p", param, &paramSign)) {

            n++; // skip the next one, that's the value of the switch we just parsed.
        } else if (extractBoolSwitch(arg, "--stats", "-s", stats)) {
            // the arguments is a switch
        } else {
            // look for level&param in the argument
            if (levelSign == 0) {
                if (!parseLong(argv[n], level)) {
                    fprintf(stderr, "Level has to be a number.\n");
                    return 0;
                }

                levelSign = 1;
            } else if (paramSign == 0) {
                if (!parseLong(argv[n], param)) {
                    fprintf(stderr, "Param has to be a number.\n");
                    return 0;
                }
                
                paramSign = 1;
            } else {
                fprintf(stderr, "Too many arguments!\n");
                return 0;
            }

            if ((paramSign != 0 && paramSign != 1) || (levelSign != 0 && levelSign != 1)) {
                fprintf(stderr, "Invalid argument %s.\n", argv[n]);
                return 0;
            }
        }

        // an error occurred
        if (levelSign == 2 || paramSign == 2) {
            return 0;
        }
    }

    // printf("Level: %ld, Param: %ld, Stats: %ld\n", *level, *param, *stats);
    return 1;
}

int checkBounds(long level, long param) {
    if (level < 0) {
        fprintf(stderr, "Invalid level. Has to be a positive number.\n");
        return 1;
    }

    if (level > 4) {
        fprintf(stderr, "Level too high. Allowed: <1; 4>\n");
        return 1;
    }

    if (level < 1) {
        fprintf(stderr, "Level too low. Allowed: <1; 4>\n");
        return 1;
    }

    // param

    if (param < 1) {
        fprintf(stderr, "Invalid param. Has to be a positive number.\n");
        return EXIT_FAILURE;
    }
    
    return 0;
}

int main(int argc, const char *argv[]) {
    long level = 1;
    long param = 1;
    bool stats = false;

    if (!extractParams(argc, argv, &level, &param, &stats)) {
        return EXIT_FAILURE;
    }

    if (checkBounds(level, param)) {
        return EXIT_FAILURE;
    }

    // stats

    char uniqueChars[128];
    unsigned long unique = 0;

    int min = 0;

    float sum = 0;
    float passwordCount = 0;

    // Load data

    while (1) {
        int read = 1;

        char buffer[101];

        int c;
        int i;

        for (i = 0;; i++) {

            if (i > 100) {
                fprintf(stderr, "Password too long. Max: 100.");
                return EXIT_FAILURE;
            }

            c = fgetc(stdin);

            if (c == EOF) {
                read = 0; // stop reading
                break;
            }

            if (c == '\n') {
                break;
            }

            // if unique, add and increase count
            if (!containsc(uniqueChars, c)) {
                uniqueChars[unique] = c;
                unique++;
            }

            buffer[i] = c;
        }

        if (!read) {
            break;
        }

        buffer[i] = '\0'; // terminate

        if (i < min || min == 0) {
            min = i; // pick a new shortest pass
        }

        passwordCount++;
        sum += i;

        // determine the level of safety

        if (level >= 1) {
            if (!containsASCIIRange(buffer, 65, 90) || !containsASCIIRange(buffer, 97, 122)) {
                // fprintf(stderr, "Missing letters.\n");
                continue;
            }
        }

        if (level >= 2) {
            long score =
                    containsASCIIRange(buffer, 65, 90) +  // uppercase alfanumeric
                    containsASCIIRange(buffer, 97, 122) + // lowercase alfanumeric
                    containsASCIIRange(buffer, 48, 57) +  // special characters
                    containsSpecialCharacters(buffer);    // numbers

            if (score < param) {
                // fprintf(stderr, "Password is not secure enough.\n");
                continue;
            }
        }

        if (level >= 3) {
            if (checkDuplicateChars(buffer, param)) {
                // fprintf(stderr, "Too many same sequential characters in the password. (%ld, threshold: %ld)\n", duplicates, param);
                continue;
            }
        }

        if (level >= 4) {
            if (containsDuplicateSubstrings(buffer, param)) {
                // fprintf(stderr, "Contains too many identical substrings of length %ld.\n", param);
                continue;
            }
        }

        printf("%s\n", buffer);
    }

    if (stats) {
        printf("Statistika:\nRuznych znaku: %ld\nMinimalni delka: %d\nPrumerna delka: %.1f\n", unique, min, (sum / passwordCount));
    }

    return 0;
}