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

#define LOWEST 1
#define EQUALS 2
#define LESSGREATER 3
#define SUM 4
#define PRODUCT 5
#define PREFIX 6
#define CALL 7

typedef char * TokenType;
typedef char * TokenLiteral;
typedef char * StatementType;
typedef char * ValueType;

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

struct PrefixExpression {
    struct Token token;
    char * operator;
    ValueType expression_type;
    void * right;
};

struct IntegerLiteral {
    struct Token token;
    int value;
};

struct Identifier {
    struct Token token;
    char * value;
};

struct ExpressionStatement {
    struct Token token;
    ValueType expression_type;
    void * expression;
};

struct LetStatement {
    struct Token token;
    struct Identifier name;
    ValueType type; // Identifier, ExpressionStatement, IntegerLiteral, ...
    void * value;
};

struct ReturnStatement {
    struct Token token;
    void * value;
    ValueType type;
};

struct Statement {
    StatementType type;
    void * st;
};

struct Program {
    struct Statement * statements;
    int sc;
};

struct Parser {
    struct Lexer * lexer;
    struct Token current_token;
    struct Token peek_token;
    char ** errors;
    int ec;
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

int str_to_int(char * str)
{
    int result;
    int puiss;
    result = 0;
    puiss = 1;

    while (('-' == (*str)) || ((*str) == '+')) {
        if (*str == '-')
            puiss = puiss * -1;

        str++;
    }

    while ((*str >= '0') && (*str <= '9')) {
        result = (result * 10) + ((*str) - '0');
        str++;
    }

    return (result * puiss);
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

struct Token lexer_next_token(struct Lexer * lexer) {
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
    struct Token token = lexer_next_token(lexer);

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

        token = lexer_next_token(lexer);
    }

    free(lexer);
}

void parser_next_token(struct Parser * parser) {
    parser->current_token = parser->peek_token;
    parser->peek_token = lexer_next_token(parser->lexer);
}

struct Parser * new_parser(struct Lexer * lexer) {
    struct Parser * parser = malloc(sizeof(struct Parser));
    parser->lexer = lexer;
    parser->errors = malloc(sizeof(char *));
    parser->ec = 0;
    parser_next_token(parser);
    parser_next_token(parser);
    return parser;
}

void peek_error(struct Parser * parser, TokenType type) {
    int ex_len = strlen(type);
    int gt_len = strlen(parser->peek_token.type);
    char * str = malloc((14+ex_len+gt_len)*sizeof(char));
    sprintf(str, "Expected %s got %s", type, parser->peek_token.type);
    parser->errors[parser->ec] = str;
    parser->errors[++parser->ec] = malloc(sizeof(char *));
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
        peek_error(parser, type);
        return false;
    }
}

void parse_let_statement(struct Parser * par, struct Statement * smt) {
    smt->st = malloc(sizeof(struct LetStatement));
    ((struct LetStatement *)smt->st)->token = par->current_token;

    if(!expect_peek(par, IDENT))
        return;

    ((struct LetStatement *)smt->st)->name.token = par->current_token;
    ((struct LetStatement *)smt->st)->name.value = par->current_token.literal;

    if(!expect_peek(par, ASSIGN))
        return;

    while(!cur_token_is(par, SEMICOLON))
        parser_next_token(par);
}

void * parse_identifier(struct Parser * par) {
    struct Identifier * identifier = malloc(sizeof(struct Identifier));
    identifier->token = par->current_token;
    identifier->value = par->current_token.literal;
    return identifier;
}

void * parse_integer_literal(struct Parser * par) {
    struct IntegerLiteral * lit = malloc(sizeof(struct IntegerLiteral));
    lit->token = par->current_token;
    lit->value = str_to_int(par->current_token.literal);
    return lit;
}

void * parse_expression(struct Parser * par, int precedence);

void * parse_prefix_expression(struct Parser * par) {
    struct PrefixExpression * pe = malloc(sizeof(struct PrefixExpression));
    pe->token = par->current_token;
    pe->operator = par->current_token.literal;
    parser_next_token(par);
    pe->right = parse_expression(par, PREFIX);
    return pe;
}

void * parse_expression(struct Parser * par, int precedence) {
    if(par->current_token.type == IDENT) {
        return parse_identifier(par);
    } else if(par->current_token.type == INT) {
        return parse_integer_literal(par);
    } else if(par->current_token.type == BANG || par->current_token.type == MINUS) {
        return parse_prefix_expression(par);
    }
}

void parse_expression_statement(struct Parser * par, struct Statement * smt) {
    smt->st = malloc(sizeof(struct ExpressionStatement));
    ((struct ExpressionStatement *)smt->st)->token = par->current_token;
    ((struct ExpressionStatement *)smt->st)->expression_type =
        par->current_token.type;
    ((struct ExpressionStatement *)smt->st)->expression =
        parse_expression(par, LOWEST);

    if(peek_token_is(par, SEMICOLON))
        parser_next_token(par);
}

void parse_return_statement(struct Parser * par, struct Statement * smt) {
    smt->st = malloc(sizeof(struct ReturnStatement));
    ((struct ReturnStatement *)smt->st)->token = par->current_token;

    parser_next_token(par);

    while(!cur_token_is(par, SEMICOLON))
        parser_next_token(par);
}

struct Program * parse_program(struct Parser * parser) {
    size_t sz;
    struct Program * prg = malloc(sizeof(struct Program));
    prg->statements = malloc(sizeof(struct Statement));
    prg->sc = 0;

    while(strcmp(parser->current_token.type, EOF_) != 0) {
        if(strcmp(parser->current_token.type, LET) == 0) {
            sz = sizeof(struct Statement) * (prg->sc+1);
            prg->statements = realloc(prg->statements, sz);
            prg->statements[prg->sc].type = LET;
            parse_let_statement(parser, &prg->statements[prg->sc++]);
        } else if(strcmp(parser->current_token.type, RETURN) == 0) {
            sz = sizeof(struct Statement) * (prg->sc+1);
            prg->statements = realloc(prg->statements, sz);
            prg->statements[prg->sc].type = RETURN;
            parse_return_statement(parser, &prg->statements[prg->sc++]);
        } else {
            sz = sizeof(struct Statement) * (prg->sc+1);
            prg->statements = realloc(prg->statements, sz);
            prg->statements[prg->sc].type = "EXPRESSION";
            parse_expression_statement(parser, &prg->statements[prg->sc++]);
        }

        parser_next_token(parser);
    }

    return prg;
}

bool check_parser_errors(struct Parser * parser) {
    int i;

    if(parser->ec == 0)
        return false;

    if(parser->ec != 0)
        for(i = 0; i < parser->ec; i++)
            printf("%s\n", parser->errors[i]);

    return true;
}

char * print_let_statement(struct LetStatement * smt) {
    int v = (strlen(smt->token.literal) + strlen(smt->name.value) + 6);
    char * let = malloc(sizeof(char) * v);
    let[0] = '\0';
    strcat(let, smt->token.literal);
    strcat(let, " ");
    strcat(let, smt->name.value);
    strcat(let, " = ");
    strcat(let, ";");
    return let;
}

char * print_return_statement(struct ReturnStatement * smt) {
    int v = (strlen(smt->token.literal) + 2);
    char * let = malloc(sizeof(char) * v);
    let[0] = '\0';
    strcat(let, smt->token.literal);
    strcat(let, ";");
    return let;
}

bool test_let_statement(struct Statement smt, const char * name) {
    struct LetStatement * ls = (struct LetStatement *)(smt.st);

    char * nv = ls->name.value;
    char * tl = ls->name.token.literal;
    char * tt = ls->token.type;

    printf("%s\n", print_let_statement(ls));

    if(strcmp(smt.type, LET) != 0)
        printf("Statement's type not \"LET\", got \"%s\"\n", smt.type);

    if(strcmp(tt, LET) != 0)
        printf("Token's type not \"LET\", got \"%s\"\n", tt);

    if(strcmp(nv, name) != 0)
        printf("Name's value not '%s', got '%s'\n", name, nv);

    if(strcmp(tl, name) != 0)
        printf("Name's token literal not '%s', got '%s'\n", name, tl);
}

void test_let_statements() {
    int i;
    const char * input = " \
            let x = 5; \
            let y = 10; \
            let foobar = 838383;";
    const char * tests[3] = {"x", "y", "foobar"};
    struct Lexer * lexer = new_lexer(input);
    struct Parser * parser = new_parser(lexer);
    struct Program * program = parse_program(parser);

    if(check_parser_errors(parser))
        return;

    if(program == NULL)
        printf("parse_program() returned NULL");

    if(program->sc != 3)
        printf("Expected 3 statements, got %i", program->sc);

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        test_let_statement(program->statements[i], tests[i]);
    }
}

void test_return_statements() {
    int i;
    struct Statement smt;
    char * tl;
    const char * input = " \
            return 5; \
            return 10; \
            return 993322;";
    const char * tests[3] = {"x", "y", "foobar"};
    struct Lexer * lexer = new_lexer(input);
    struct Parser * parser = new_parser(lexer);
    struct Program * program = parse_program(parser);

    if(check_parser_errors(parser))
        return;

    if(program->sc != 3)
        printf("Expected 3 statements, got %i", program->sc);

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        smt = program->statements[i];
        tl = ((struct ReturnStatement *)(smt.st))->token.literal;

        if(strcmp(tl, "return") != 0)
            printf("Token literal not \"RETURN\", got \"%s\"\n", tl);

        if(strcmp(smt.type, RETURN) != 0)
            printf("Statement's type not \"RETURN\", got \"%s\"\n", smt.type);
    }
}

void test_identifier_expression() {
    struct Statement smt;
    char * tl;
    struct ExpressionStatement * e_st;
    struct Identifier * id;
    const char * input = "foobar;";
    struct Lexer * lexer = new_lexer(input);
    struct Parser * parser = new_parser(lexer);
    struct Program * program = parse_program(parser);

    if(check_parser_errors(parser))
        return;

    if(program->sc != 1)
        printf("Expected 1 statements, got %i", program->sc);

    smt = program->statements[0];
    e_st = ((struct ExpressionStatement *)(smt.st));
    id = ((struct Identifier *)(e_st->expression));
    tl = e_st->token.literal;

    if(strcmp(e_st->expression_type, "IDENT") != 0)
        printf("Expression type not \"IDENT\", got \"%s\"\n",
            e_st->expression_type);

    if(strcmp(tl, "foobar") != 0)
        printf("Token literal not \"foobar\", got \"%s\"\n", tl);

    if(strcmp(tl, "foobar") != 0)
        printf("Identifier value not \"foobar\", got \"%s\"\n", id->value);

    if(strcmp(smt.type, "EXPRESSION") != 0)
        printf("Statement's type not \"EXPRESSION\", got \"%s\"\n", smt.type);
}

void test_integer_literal_expression() {
    struct Statement smt;
    struct ExpressionStatement * est;
    struct IntegerLiteral * il;
    const char * input = "5;";
    struct Lexer * lexer = new_lexer(input);
    struct Parser * parser = new_parser(lexer);
    struct Program * program = parse_program(parser);

    if(check_parser_errors(parser))
        return;

    if(program->sc != 1)
        printf("Expected 1 statements, got %i", program->sc);

    smt = program->statements[0];
    est = ((struct ExpressionStatement *)(smt.st));

    printf("%s", est->token.literal);
    il = ((struct IntegerLiteral *)(est->expression));

    if(il->value != 5)
        printf("Integer literal's value not 5, got %i\n", il->value);

    if(strcmp(smt.type, "EXPRESSION") != 0)
        printf("Statement's type not \"EXPRESSION\", got \"%s\"\n", smt.type);
}

void * test_parsing_prefix_expressions() {
    struct T {
        char * input;
        char * operator;
        int integer_value;
    };
    int i;
    struct T tests[2] = {{"!5", "!", 5}, {"-15", "-", 15}};

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        struct Lexer * lexer = new_lexer(tests[i].input);
        struct Parser * parser = new_parser(lexer);
        struct Program * program = parse_program(parser);

        if(check_parser_errors(parser))
            break;

        if(program->sc != 1)
            printf("Expected 1 statements, got %i", program->sc);

        if(strcmp(program->statements[0].type, "EXPRESSION") != 0)
            printf("Expression type not \"EXPRESSION\", got \"%s\"\n",
                program->statements[0].type);

        struct ExpressionStatement * est =
            ((struct ExpressionStatement *)(program->statements[0].st));

        struct PrefixExpression * pes =
            ((struct PrefixExpression *)(est->expression));

        struct IntegerLiteral * il =
            ((struct IntegerLiteral *)(pes->right));

        printf("%i\n", il->value);
    }
}

int main(int argc, char * argv[])
{
    if(strcmp(argv[1], "test-parsing-prefix-expressions") == 0)
        test_parsing_prefix_expressions();

    if(strcmp(argv[1], "test-integer-literal") == 0)
        test_integer_literal_expression();

    if(strcmp(argv[1], "test-identifier-expression") == 0)
        test_identifier_expression();

    if(strcmp(argv[1], "test-return-statements") == 0)
        test_return_statements();

    if(strcmp(argv[1], "test-next-token") == 0)
        test_next_token();

    if(strcmp(argv[1], "test-let-statements") == 0)
        test_let_statements();

    if(strcmp(argv[1], "parser") == 0) {
        const char * input = "let x = 5; let a = 3;";

        struct Lexer * lexer = new_lexer(input);
        struct Parser * parser = new_parser(lexer);
        struct Program * program = parse_program(parser);
        struct Statement statement = program->statements[0];

        printf("%s\n", statement.type);
        printf("%s", ((struct LetStatement *)(statement.st))->name.value);
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
                token = lexer_next_token(lexer);
                printf("{Literal: %s, Type: %s}\n", token.literal, token.type);
            }
        }
    }

    return 0;
}
