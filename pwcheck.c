#include <stdio.h>
#include <stdlib.h>

int length(char *str)
{
    int n;
    for (n = 0; str[n] != '\0'; n++)
    {
    }
    return n;
}

int cmp(char *str1, char *str2)
{
    for (int i = 0; str1[i] != '\0' || str2[i] != '\0'; i++)
    {
        if (str1[i] != str2[i])
        {
            return 0;
        }
    }

    return 1;
}

int containsc(char *where, char what)
{
    for (int i = 0; where[i] != '\0'; i++)
    {
        if (what == where[i])
        {
            return 1;
        }
    }

    return 0;
}

// find a string in another
// return the count of findings
int find(char *where, char *what)
{
    int n = 0;

    int count = 0;

    for (int i = 0; where[i] != '\0'; i++)
    {
        if (what[n] == where[i])
        {
            n++;

            if (what[n] == '\0') // reached the end (aka found whole string)
            {
                count += 1;

                i -= n - 1; // move back to the start of the matched string
                n = 0;
            }
        }
        else
        {
            n = 0; // reset to 0
        }
    }

    return count;
}

// check whether a string contains at least one character the an ASCII range specified by low & high
int containsASCIIRange(char *str, int low, int high)
{
    for (int n = 0; str[n] != '\0'; n++)
    {
        if (str[n] >= low && str[n] <= high)
        {
            return 1;
        }
    }

    return 0;
}

// check whether a string contains any special ASCII characters
int containsSpecialCharacters(char *str)
{
    return (containsASCIIRange(str, 33, 47) ||
            containsASCIIRange(str, 58, 64) ||
            containsASCIIRange(str, 91, 96) ||
            containsASCIIRange(str, 123, 126))
               ? 1
               : 0;
}

// count how many duplicate sequential characters there are in a given string, until a threshold is reached
int checkDuplicateChars(char *str, int threshold)
{
    char last = str[0];
    int count = 0;

    for (int i = 1; str[i] != '\0'; i++)
    {
        if (str[i] == last)
        {
            count += 1;
        }
        else
        {
            count = 0;
        }

        last = str[i];

        if (count >= threshold - 1)
        {
            return 1;
        }
    }

    return 0;
}

// slice a string, return into @param buff
void slice(char *str, char *buff, int start, int end)
{
    for (int n = start; str[n] != '\0' && n < end; n++)
    {
        buff[n - start] = str[n];
    }

    buff[end - start] = '\0'; // terminate
}

// check whether a string contains any duplicate substrings of given length
int containsDuplicateSubstrings(char *str, int subLength)
{
    int len = length(str);

    // hold the substring we're looking for
    char buff[subLength];

    for (int n = 0; n < len - subLength + 1; n++)
    {
        slice(str, buff, n, n + subLength);

        if (find(str, buff) > 1) // found more than the original
        {
            return 1; // return true
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Not enough arguments.");
        return EXIT_FAILURE;
    }

    if (argc > 4)
    {
        fprintf(stderr, "Too many arguments.");
        return EXIT_FAILURE;
    }

    char *nonNum;

    long level = 1;

    // level

    // level
    level = strtol(argv[1], &nonNum, 10);

    if (nonNum[0] != '\0')
    {
        fprintf(stderr, "Invalid level. Has to be a number.\n");
        return EXIT_FAILURE;
    }

    if (level < 0)
    {
        fprintf(stderr, "Invalid level. Has to be a positive number.\n");
        return EXIT_FAILURE;
    }

    if (level > 4)
    {
        fprintf(stderr, "Level too high. Allowed: <1; 4>\n");
        return EXIT_FAILURE;
    }

    if (level < 1)
    {
        fprintf(stderr, "Level too low. Allowed: <1; 4>\n");
        return EXIT_FAILURE;
    }

    // param

    long param;

    param = strtol(argv[2], &nonNum, 10);

    if (nonNum[0] != '\0')
    {
        fprintf(stderr, "Invalid param. Has to be a number.\n");
        return EXIT_FAILURE;
    }

    if (param < 1)
    {
        fprintf(stderr, "Invalid param. Has to be a positive number.\n");
        return EXIT_FAILURE;
    }

    // stats

    int stats = 0;

    if (argc > 3)
    {
        char *buff = argv[3];
        if (cmp(buff, "--stats"))
        {
            stats = 1;
        }
        else
        {
            fprintf(stderr, "Invalid argument.");
            return EXIT_FAILURE;
        }
    }

    char uniqueChars[128];
    unsigned long unique = 0;
    int min = 0;

    float sum = 0;
    float passwordCount = 0;

    // Load data

    while (1)
    {
        int read = 1;

        char buffer[101];

        int c;
        int i;

        for (i = 0;; i++)
        {

            if (i > 100)
            {
                fprintf(stderr, "Password too long. Max: 100.");
                return EXIT_FAILURE;
            }

            c = fgetc(stdin);

            if (c == EOF)
            {
                read = 0; // stop reading
                break;
            }

            if (c == '\n')
            {
                break;
            }

            // if unique, add and increase count
            if (read && containsc(uniqueChars, c) == 0)
            {
                uniqueChars[unique] = c;
                unique++;
            }

            buffer[i] = c;
        }

        if (!read)
        {
            break;
        }

        buffer[i] = '\0'; // terminate

        if (i < min || min == 0)
        {
            min = i; // pick a new shortest pass
        }

        passwordCount++;
        sum += i;

        // determine the level of safety

        if (level >= 1)
        {
            if (!containsASCIIRange(buffer, 65, 90) || !containsASCIIRange(buffer, 97, 122))
            {
                // fprintf(stderr, "Missing letters.\n");
                continue;
            }
        }

        if (level >= 2)
        {
            long score;

            score = containsASCIIRange(buffer, 65, 90) +  // uppercase alfanumeric
                    containsASCIIRange(buffer, 97, 122) + // lowercase alfanumeric
                    containsASCIIRange(buffer, 48, 57) +  // special characters
                    containsSpecialCharacters(buffer);    // numbers

            if (score < param)
            {
                // fprintf(stderr, "Password is not secure enough.\n");
                continue;
            }
        }

        if (level >= 3)
        {
            if (checkDuplicateChars(buffer, param))
            {
                // fprintf(stderr, "Too many same sequential characters in the password. (%ld, threshold: %ld)\n", duplicates, param);
                continue;
            }
        }

        if (level >= 4)
        {
            if (containsDuplicateSubstrings(buffer, param))
            {
                // fprintf(stderr, "Contains too many identical substrings of length %ld.\n", param);
                continue;
            }
        }

        printf("%s\n", buffer);
    }

    if (stats)
    {
        printf("Statistika:\nRuznych znaku: %ld\nMinimalni delka: %d\nPrumerna delka: %.1f\n", unique, min, (sum / passwordCount));
    }

    return 0;
}