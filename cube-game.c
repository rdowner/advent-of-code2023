
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"

struct game_round {
    uint_least8_t red, green, blue;
};
typedef struct game_round game_round;
struct game_result {
    int game_number;
    int possible;
    int power;
};
typedef struct game_result game_result;

enum token_type { game = 1, red = 2, green = 4, blue = 8, number = 16, space = 32, colon = 64, semicolon = 128, comma = 256, eol = 512 };
typedef enum token_type token_type;

struct token {
    token_type type;
    union {
        int number;
        int nspaces;
    } value;
};
typedef struct token token;

game_result *run_next_game(char *line, int line_number);
int next_token(char *start, int line_number, char **in, token *token);
int try_next_token(char *start, int line_number, char **in, token *token, int expectations);
void expect_next_token(char *start, int line_number, char **in, token *token, int expectations);

int main(void)
{
    int running_total = 0, power_sum = 0;
    char *line;
    int line_number = 0;
    while (line = stdin_line_read()) {
        line_number++;
        game_result *gr = run_next_game(line, line_number);
        if (gr->possible)
            running_total += gr->game_number;
        power_sum += gr->power;
        free(gr);
        free(line);
    }
    printf("Result: %d\nPower sum: %d\n", running_total, power_sum);
    return 0;
}

game_result *run_next_game(char *line, int line_number)
{
    char *start = line;
    char *ptr = line;

    token token;
    expect_next_token(start, line_number, &ptr, &token, game);
    expect_next_token(start, line_number, &ptr, &token, number);
    int game_number = token.value.number;
    expect_next_token(start, line_number, &ptr, &token, colon);

    int game_impossible = 0, game_done = 0;
    int max_red = 0, max_green = 0, max_blue = 0;
    while(!game_done) {
        expect_next_token(start, line_number, &ptr, &token, number);
        game_round round;
        memset(&round, 0, sizeof(round));
        for(int round_done = 0; round_done == 0;) {
            int count = token.value.number;
            expect_next_token(start, line_number, &ptr, &token, red | green | blue);
            switch(token.type) {
                case red  : round.red = count; max_red = max(max_red, count); break;
                case green: round.green = count; max_green = max(max_green, count); break;
                case blue : round.blue = count; max_blue = max(max_blue, count); break;
            }
            expect_next_token(start, line_number, &ptr, &token, comma | semicolon | eol);
            switch(token.type) {
                case comma:
                    expect_next_token(start, line_number, &ptr, &token, number);
                    break;
                case eol:
                    game_done = 1;
                    /* fall through */
                case semicolon:
                    round_done = 1;
                    break;
            }
        }
        if (round.red > 12 || round.green > 13 || round.blue > 14)
            game_impossible = 1;
    }

    int power = max_red*max_green*max_blue;
    printf("Game %d is %s; min red=%d green=%d blue=%d power=%d\n", game_number, game_impossible ? "impossible" : "possible", max_red, max_green, max_blue, power);
    game_result *gr = calloc(sizeof(game_result), 1);
    gr->game_number = game_number;
    gr->possible = !game_impossible;
    gr->power = power;
}

int next_token(char *start, int line_number, char **in, token *token)
{
    char *ptr = *in;
    char cur = *ptr;

    if (isalpha(cur)) {
        size_t len;
        for (len = 0; isalpha(ptr[len]); len++) ;
        if (strncmp(ptr, "Game", len) == 0)
            token->type = game;
        else if (strncmp(ptr, "red", len) == 0)
            token->type = red;
        else if (strncmp(ptr, "green", len) == 0)
            token->type = green;
        else if (strncmp(ptr, "blue", len) == 0)
            token->type = blue;
        else {
            printf("unknown word: %s\n", ptr);
            exit(1);
        }
        ptr += len;
    } else if (isdigit(cur)) {
        token->type = number;
        token->value.number = strtol(ptr, &ptr, 10);
    } else if (isspace(cur)) {
        int len;
        for (len = 0; isspace(ptr[len]); len++) ;
        token->type = space;
        token->value.nspaces = len;
        ptr += len;
    } else if (cur == ':') {
        token->type = colon;
        ptr++;
    } else if (cur == ';') {
        token->type = semicolon;
        ptr++;
    } else if (cur == ',') {
        token->type = comma;
        ptr++;
    } else if (cur == '\n' || cur == '\0') {
        token->type = eol;
        ptr++;
    } else {
        return 0;
    }
    *in = ptr;
    return 1;
}

int try_next_token(char *start, int line_number, char **in, token *token, int expectations)
{
    char *ptr = *in;
    do {
        int success = next_token(start, line_number, in, token);
        if (!success) {
            return 0;
        }
    } while (token->type == space);
    if ((token->type & expectations) == 0) {
        printf("unexpected token at line %d col %ld, expected_mask %u, actual %u: %s\n", line_number, *in - start, expectations, token->type, *in);
        exit(1);
    }
    return 1;
}

void expect_next_token(char *start, int line_number, char **in, token *token, int expectations)
{
    if(!try_next_token(start, line_number, in, token, expectations)) {
        printf("illegal token at line %d col %ld: %s\n", line_number, 1 + *in - start, *in);
        exit(1);
    }
}
