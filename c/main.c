/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stdbool.h>

#define ILLEGAL "ILLEGAL"
#define EOF_ "EOF"
#define IDENT "IDENT"
#define INT "INT"
#define ASSIGN "="
#define PLUS "+"
#define MINUS "-"
#define BANG "!"
#define ASTERISK "*"
#define SLASH "/"
#define LT "<"
#define GT ">"
#define COMMA ","
#define SEMICOLON ";"
#define FUNCTION "FUNCTION"
#define LET "LET"
#define LPAREN "("
#define RPAREN ")"
#define LBRACE "{"
#define RBRACE "}"
#define ILLEGAL "ILLEGAL"
#define TRUE "TRUE"
#define FALSE "FALSE"
#define IF "IF"
#define ELSE "ELSE"
#define RETURN "RETURN"
#define EQ "=="
#define NOT_EQ "!="

typedef char * TokenType;
typedef char * TokenLiteral;

struct Token {
    TokenType type;
    char * literal;
};

struct Lexer {
    char ch;
    const char * input;
    int pos;
    int read_pos;
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

char * get_substr(int position, int final_pos, const char * input) {
    int i;
    int size = final_pos-position;
    char * buff = malloc(sizeof(char) * size + 1);

    for(i = 0; position < final_pos; position++, i++)
        buff[i] = input[position];

    buff[size] = '\0';
    return buff;
}

bool is_letter(char ch) {
    return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_';
}

bool is_digit(char ch) {
    return '0' <= ch && ch <= '9';
}

char * read_identifier(struct Lexer * lexer) {
    int position = lexer->pos;

    while(is_letter(lexer->ch))
        read_char(lexer);

    return get_substr(position, lexer->pos, lexer->input);
}

char * read_number(struct Lexer * lexer) {
    int position = lexer->pos;

    while(is_digit(lexer->ch))
        read_char(lexer);

    return get_substr(position, lexer->pos, lexer->input);
}

char * lookup_ident(char * literal) {
    if(strcmp("fn", literal) == 0)
        return FUNCTION;
    else if(strcmp("let", literal) == 0)
        return LET;
    else if(strcmp("true", literal) == 0)
        return TRUE;
    else if(strcmp("false", literal) == 0)
        return FALSE;
    else if(strcmp("if", literal) == 0)
        return IF;
    else if(strcmp("else", literal) == 0)
        return ELSE;
    else if(strcmp("return", literal) == 0)
        return RETURN;
    else
        return IDENT;
}

void skip_whitespace(struct Lexer * lexer) {
    while(lexer->ch == ' ' || lexer-> ch == '\t' || lexer->ch == '\n'
        || lexer->ch  == '\r') {
        read_char(lexer);
    }
}

char peek_char(struct Lexer * lexer) {
    if(lexer->read_pos >= strlen(lexer->input))
        return 0;
    else
        return lexer->input[lexer->read_pos];
}

struct Token next_token(struct Lexer * lexer) {
    struct Token token;

    skip_whitespace(lexer);

    switch(lexer->ch) {
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
    case '-':
        token = new_token(MINUS, lexer->ch);
        break;
    case '*':
        token = new_token(ASTERISK, lexer->ch);
        break;
    case '/':
        token = new_token(SLASH, lexer->ch);
        break;
    case '<':
        token = new_token(LT, lexer->ch);
        break;
    case '>':
        token = new_token(GT, lexer->ch);
        break;
    case '=':
        if(peek_char(lexer) == '=') {
            read_char(lexer);
            token.type = EQ;
            token.literal = "==";
        } else {
            token = new_token(ASSIGN, lexer->ch);
        }
        break;
    case '!':
        if(peek_char(lexer) == '=') {
            read_char(lexer);
            token.type = NOT_EQ;
            token.literal = "!=";
        } else {
            token = new_token(BANG, lexer->ch);
        }
        break;
    case 0:
        token.literal = "";
        token.type = EOF_;
        break;
    default:
        if(is_letter(lexer->ch)) {
            token.literal = read_identifier(lexer);
            token.type = lookup_ident(token.literal);
            return token;
        } else if(is_digit(lexer->ch)) {
            token.literal = read_number(lexer);
            token.type = INT;
            return token;
        } else {
            token = new_token(ILLEGAL, lexer->ch);
        }
    };

    read_char(lexer);
    return token;
}

void test_next_token() {
    int i;
    const char * input =
        "let x = 5; \
        let y = 10; \
        let foobar = 838383;";
    const char * tests[16][2] = {
        {LET, "let"},
        {IDENT, "x"},
        {ASSIGN, "="},
        {INT, "5"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "y"},
        {ASSIGN, "="},
        {INT, "10"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "foobar"},
        {ASSIGN, "="},
        {INT, "838383"},
        {SEMICOLON, ";"},
        {EOF_, ""}};
    struct Lexer * lexer = new_lexer(input);
    struct Token token = next_token(lexer);

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        printf("%s %s '%c' %i %i \n", token.literal, token.type, lexer->ch,
            lexer->pos, lexer->read_pos);

        if(strcmp(token.type, tests[i][0]) != 0)
            printf("[%i] Token type wrong: expected %s got %s \n",
                i, tests[i][0], token.type);

        if(strcmp(token.literal, tests[i][1]) != 0)
            printf("[%i] Token literal wrong: expected %s got %s \n",
                i, tests[i][1], token.literal);

        if(token.type == LET || token.type == FUNCTION
            || token.type == IDENT || token.type == INT)
            free(token.literal);

        token = next_token(lexer);
    }

    free(lexer);
}

struct Identifier {
    struct Token token;
    char * value;
};

struct LetStatement {
    struct Token token;
    struct Identifier name;
};

struct Statement {
    char * type;
    TokenLiteral literal;
    struct LetStatement * let_statement;
};

struct Program {
    struct Statement * statements;
    int sc;
};

struct Parser {
    struct Lexer * lexer;
    struct Token current_token;
    struct Token peek_token;
};

char * token_literal(struct Program * program) {
    size_t size = sizeof(program->statements) / sizeof(program->statements[0]);

    if(size > 0)
        return program->statements[0].literal;
    else
        return "";
}

void parser_next_token(struct Parser * parser) {
    parser->current_token = parser->peek_token;
    parser->peek_token = next_token(parser->lexer);
}

struct Parser * new_parser(struct Lexer * lexer) {
    struct Parser * parser = malloc(sizeof(struct Parser));
    parser->lexer = lexer;
    parser_next_token(parser);
    parser_next_token(parser);
    return parser;
}

bool cur_token_is(struct Parser * parser, TokenType type) {
    return strcmp(parser->current_token.type, type) == 0;
}

bool peek_token_is(struct Parser * parser, TokenType type) {
    return strcmp(parser->peek_token.type, type) == 0;
}

bool expect_peek(struct Parser * parser, TokenType type) {
    if(peek_token_is(parser, type)) {
        parser_next_token(parser);
        return true;
    } else {
        return false;
    }
}

struct LetStatement * parse_let_statement(struct Parser * parser) {
    struct LetStatement * statement = malloc(sizeof(struct LetStatement));

    if(!expect_peek(parser, IDENT))
        return NULL;

    statement->name.token = parser->current_token;
    statement->name.value = parser->current_token.literal;

    if(!expect_peek(parser, ASSIGN))
        return NULL;

    while(!cur_token_is(parser, SEMICOLON))
        parser_next_token(parser);

    return statement;
}

struct Program * parse_program(struct Parser * parser) {
    struct Program * program = malloc(sizeof(struct Program));
    program->statements = malloc(sizeof(struct Statement));
    int i = 0;

    while(strcmp(parser->current_token.type, EOF_) != 0) {
        if(strcmp(parser->current_token.type, LET) == 0) {
            program->statements = realloc(program->statements,
                sizeof(struct Statement) * (i+1));
            program->statements[i].type = LET;
            program->statements[i].literal = parser->current_token.literal;
            program->statements[i].let_statement = parse_let_statement(parser);
            program->sc++;
            i++;
        }
        parser_next_token(parser);
    }

    return program;
}
int main(int argc, char * argv[])
{
    if(strcmp(argv[1], "parser") == 0) {
        const char * input = "let x = 5; let y = 3;";
        struct Lexer * lexer = new_lexer(input);
        struct Parser * parser = new_parser(lexer);
        struct Program * program = parse_program(parser);

        printf("%s", program->statements[0].let_statement->name.token.literal);
    }

    if(strcmp(argv[1], "repl") == 0) {
        while(1) {
            char str[120];
            struct Lexer * lexer;
            struct Token token;
            printf(">> ");
            fgets(str, 120, stdin);
            lexer = new_lexer(str);
            token.type = ILLEGAL;

            while(strcmp(token.type, EOF_) != 0) {
                token = next_token(lexer);
                printf("{Literal: %s, Type: %s}\n", token.literal, token.type);

                if(token.type == LET || token.type == FUNCTION
                    || token.type == IDENT || token.type == INT)
                    free(token.literal);
            }

            free(lexer);
        }
    }

    if(strcmp(argv[1], "test-next-token") == 0)
        test_next_token();

    return 0;
}
