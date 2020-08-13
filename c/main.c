#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define ILLEGAL "ILLEGAL"
#define EOF_ "EOF"
#define IDENT "IDENT"
#define INT "INT"
#define ASSIGN "="
#define PLUS "+"
#define COMMA ","
#define SEMICOLON ";"
#define FUNCTION "FUNCTION"
#define LET "LET"
#define LPAREN "("
#define RPAREN ")"
#define LBRACE "{"
#define RBRACE "}"

typedef char * TokenType;

struct Token {
    TokenType type;
    char * literal;
};

struct Token new_token(TokenType type, char ch) {
    char a[2];
    struct Token token;

    a[0] = ch;
    a[1] = '\0';

    token.type = type;
    token.literal = a;

    return token;
}

struct Lexer {
    char ch;
    const char * input;
    int pos;
    int read_pos;
};

void read_char(struct Lexer * lexer) {
    if(lexer->read_pos >= strlen(lexer->input))
        lexer->ch = 0;
    else
        lexer->ch = lexer->input[lexer->read_pos];

    lexer->pos = lexer->read_pos;
    lexer->read_pos += 1;
}

struct Lexer * new_lexer(const char * input) {
    struct Lexer * lexer = malloc(sizeof(struct Lexer));

    lexer->input = input;
    lexer->pos = 0;
    lexer->read_pos = 0;

    read_char(lexer);

    return lexer;
}

struct Token next_token(struct Lexer * lexer) {
    struct Token token;

    switch(lexer->ch) {
    case '=':
        token = new_token(ASSIGN, lexer->ch);
        break;
    case ';':
        token = new_token(SEMICOLON, lexer->ch);
        break;
    case '(':
        token = new_token(LPAREN, lexer->ch);
        break;
    case ')':
        token = new_token(RPAREN, lexer->ch);
        break;
    case ',':
        token = new_token(COMMA, lexer->ch);
        break;
    case '+':
        token = new_token(PLUS, lexer->ch);
        break;
    case '{':
        token = new_token(LBRACE, lexer->ch);
        break;
    case '}':
        token = new_token(RBRACE, lexer->ch);
        break;
    case 0:
        token.literal = "";
        token.type = EOF_;
        break;
    };

    read_char(lexer);

    return token;
}

void test_next_token() {
    int i;

    const char * input = "=+(){},;";
    const char * tests[9][2] = {
        {ASSIGN, "="},
        {PLUS, "+"},
        {LPAREN, "("},
        {RPAREN, ")"},
        {LBRACE, "{"},
        {RBRACE, "}"},
        {COMMA, ","},
        {SEMICOLON, ";"},
        {EOF_, ""}};

    struct Lexer * lexer = new_lexer(input);
    struct Token token;

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        token = next_token(lexer);

        if(strcmp(token.type, tests[i][0]) != 0)
            printf("[%i] Token type wrong: expected %s got %s \n",
                i, tests[i][0], token.type);

        if(strcmp(token.literal, tests[i][1]) != 0)
            printf("[%i] Token literal wrong: expected %s got %s \n",
                i, tests[i][1], token.literal);
    }
}

int main()
{
    struct Lexer * lexer = new_lexer("asd");
    read_char(lexer);
    free(lexer);

    test_next_token();
}
