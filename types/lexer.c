/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * types/lexer.c
 *
 */

typedef struct {
    char * type;
    char * literal;
} Token;

typedef struct {
    char ch;
    const char * input;
    int pos;
    int read_pos;
} Lexer;

typedef struct {
    Lexer * lexer;
    Token current_token;
    Token peek_token;
    char ** errors;
    int ec;
} Parser;

typedef struct {
    char * type;
    void * st;
} Statement;
