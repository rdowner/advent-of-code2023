

#include <stdio.h>
#include <errno.h>
#include <string.h>

/* Prototypes */
int get_calibration_value(char *line);

const char *spelled_out[] = {
    "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
};

int main(void)
{
    char line[100];

    int lineNo = 1, sum = 0;
    for(;;) {
        char *ret = fgets(line, sizeof(line), stdin);

        /* check for errors/eof */
        if (ret == NULL) {
            if (feof(stdin)) {
                /* reached end of file */
                break;
            } else {
                /* some other error */
                printf("Error reading line %d\n", lineNo);
                return 1;
            }
        }

        /* check if we got a whole line or not */
        if (strchr(line, '\n') == NULL) {
            /* line too long */
            printf("Error: line %d longer than the buffer (%d)\n", lineNo, sizeof(line));
            return 1;
        }

        int cv = get_calibration_value(line);
        printf("%2d <- %s", cv, line);
        sum += cv;

        lineNo++;
    }

    printf("Final answer: %d\n", sum);
    
    return 0;
}

int get_calibration_value(char *line)
{
    char valStr[3] = { -1, -1, 0 };

    for(char *ptr = line; *ptr != 0; ptr++) {
        char got = -1;
        if (isdigit(*ptr)) {
            got = *ptr;
        } else {
            for(int i = 0; i < 9; i++) {
                size_t len = strlen(spelled_out[i]);
                if (strncmp(ptr, spelled_out[i], len) == 0) {
                    got = '1' + i;
                    break;
                }
            }
        }
        if (got >= 0) {
            if (valStr[0] == -1) {
                /* set the first digit */
                valStr[0] = got;
            }
            /* set the last digit */
            valStr[1] = got;
        }
    }

    if (valStr[0] == -1 || valStr[1] == -1) {
        printf("bad input: %s\n", line);
        return 0;
    }

    int val = strtol(valStr, NULL, 10);
    if(errno == ERANGE) {
        printf("out of range: %s from line: %s\n", valStr, line);
        return 0;
    }
    if(errno == EINVAL) {
        printf("invalid: %s from line: %s\n", valStr, line);
        return 0;
    }

    return val;
}
