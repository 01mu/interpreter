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

typedef struct Token_ {
    char * type;
    char * literal;
} Token;

typedef struct Lexer_ {
    char ch;
    const char * input;
    int pos;
    int read_pos;
} Lexer;

typedef struct InfixExpression_ {
    Token token;
    char * operator;
    char * left_expression_type;
    char * right_expression_type;
    void * left;
    void * right;
} InfixExpression;

typedef struct PrefixExpression_ {
    Token token;
    char * operator;
    char * expression_type;
    void * right;
} PrefixExpression;

typedef struct IntegerLiteral_ {
    Token token;
    int value;
} IntegerLiteral;

typedef struct Identifier_ {
    Token token;
    char * value;
} Identifier;

typedef struct ExpressionStatement_ {
    Token token;
    char * expression_type;
    void * expression;
} ExpressionStatement;

typedef struct LetStatement_ {
    Token token;
    Identifier name;
    char * type;
    void * value;
} LetStatement;

typedef struct ReturnStatement_ {
    Token token;
    char * type;
    void * value;
} ReturnStatement;

typedef struct Statement_ {
    char * type;
    void * st;
} Statement;

typedef struct Program_ {
    Statement * statements;
    int sc;
} Program;

typedef struct Parser_ {
    Lexer * lexer;
    Token current_token;
    Token peek_token;
    char ** errors;
    int ec;
} Parser;

Token new_token(char * type, char ch) {
    Token token;

    token.type = type;
    token.literal = malloc(sizeof(char) * 2);
    token.literal[0] = ch;
    token.literal[1] = '\0';

    return token;
}

void read_char(Lexer * lexer) {
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

Lexer * new_lexer(const char * input) {
    Lexer * lexer = malloc(sizeof(Lexer));
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

char * read_identifier(Lexer * lexer) {
    int position = lexer->pos;

    while(is_letter(lexer->ch))
        read_char(lexer);

    return get_substr(position, lexer->pos, lexer->input);
}

char * read_number(Lexer * lexer) {
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

void skip_whitespace(Lexer * lexer) {
    while(lexer->ch == ' ' || lexer-> ch == '\t' || lexer->ch == '\n'
        || lexer->ch  == '\r') {
        read_char(lexer);
    }
}

char peek_char(Lexer * lexer) {
    if(lexer->read_pos >= strlen(lexer->input))
        return 0;
    else
        return lexer->input[lexer->read_pos];
}

Token lexer_next_token(Lexer * lexer) {
    Token token;

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

void parser_next_token(Parser * parser) {
    parser->current_token = parser->peek_token;
    parser->peek_token = lexer_next_token(parser->lexer);
}

Parser * new_parser(Lexer * lexer) {
    Parser * parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->errors = malloc(sizeof(char *));
    parser->ec = 0;
    parser_next_token(parser);
    parser_next_token(parser);
    return parser;
}

void peek_error(Parser * parser, char * type) {
    int ex_len = strlen(type);
    int gt_len = strlen(parser->peek_token.type);
    char * str = malloc((14+ex_len+gt_len)*sizeof(char));
    sprintf(str, "Expected %s got %s", type, parser->peek_token.type);
    parser->errors[parser->ec] = str;
    parser->errors[++parser->ec] = malloc(sizeof(char *));
}

bool cur_token_is(Parser * parser, char * type) {
    return strcmp(parser->current_token.type, type) == 0;
}

bool peek_token_is(Parser * parser, char * type) {
    return strcmp(parser->peek_token.type, type) == 0;
}

bool expect_peek(Parser * parser, char * type) {
    if(peek_token_is(parser, type)) {
        parser_next_token(parser);
        return true;
    } else {
        peek_error(parser, type);
        return false;
    }
}

void parse_let_statement(Parser * par, Statement * smt) {
    smt->st = malloc(sizeof(LetStatement));
    ((LetStatement *)smt->st)->token = par->current_token;

    if(!expect_peek(par, IDENT))
        return;

    ((LetStatement *)smt->st)->name.token = par->current_token;
    ((LetStatement *)smt->st)->name.value = par->current_token.literal;

    if(!expect_peek(par, ASSIGN))
        return;

    while(!cur_token_is(par, SEMICOLON))
        parser_next_token(par);
}

void * parse_identifier(Parser * par) {
    Identifier * identifier = malloc(sizeof(Identifier));
    identifier->token = par->current_token;
    identifier->value = par->current_token.literal;
    return identifier;
}

void * parse_integer_literal(Parser * par) {
    IntegerLiteral * lit = malloc(sizeof(IntegerLiteral));
    lit->token = par->current_token;
    lit->value = str_to_int(par->current_token.literal);
    return lit;
}

void * parse_expression(Parser * par, int precedence, ExpressionStatement * est);

int parser_get_precedence(Parser * par, char * type) {
    char * pt;

    if(strcmp(type, "peek") == 0)
        pt = par->peek_token.type;
    else if(strcmp(type, "current") == 0)
        pt = par->current_token.type;

    if(strcmp(pt, EQ) == 0 || strcmp(pt, NOT_EQ) == 0) {
        return EQUALS;
    } else if(strcmp(pt, LT) == 0 || strcmp(pt, GT) == 0) {
        return LESSGREATER;
    } else if(strcmp(pt, PLUS) == 0 || strcmp(pt, MINUS) == 0) {
        return SUM;
    } else if(strcmp(pt, SLASH) == 0 || strcmp(pt, ASTERISK) == 0) {
        return PRODUCT;
    }

    return LOWEST;
};

void * parse_prefix_expression(Parser * par) {
    PrefixExpression * pe = malloc(sizeof(PrefixExpression));
    pe->token = par->current_token;
    pe->operator = par->current_token.literal;
    parser_next_token(par);
    pe->right = parse_expression(par, PREFIX, NULL);
    pe->expression_type = par->current_token.type;
    return pe;
}

void * parse_infix_expression(Parser * par, void * left, char * zz) {
    InfixExpression * ie = malloc(sizeof(InfixExpression));
    int precedence = parser_get_precedence(par, "current");

    ie->token = par->current_token;
    ie->operator = par->current_token.literal;
    ie->left = left;
    ie->left_expression_type = zz;

    parser_next_token(par);

    ie->right_expression_type = par->current_token.type;
    ie->right = parse_expression(par, precedence, NULL);

    return ie;
}

void * parse_expression(Parser * par, int precedence, ExpressionStatement * est) {
    char * type = par->current_token.type;
    void * expr;
    char * t;

    if(type == IDENT) {
        t = IDENT;
        expr = parse_identifier(par);
    } else if(type == INT) {
        t = INT;
        expr = parse_integer_literal(par);
    } else if(type == BANG || type == MINUS) {
        expr = parse_prefix_expression(par);
        t = "PREFIX";
    }

    while(!peek_token_is(par, SEMICOLON) &&
        precedence < parser_get_precedence(par, "peek")) {

        type = par->peek_token.type;

        if(type == PLUS || type == MINUS || type == SLASH ||
            type == ASTERISK || type == EQ || type == NOT_EQ ||
            type == LT || type == GT) {

            t = "INFIX";
            char * zz = par->current_token.type;
            parser_next_token(par);
            expr = parse_infix_expression(par, expr, zz);
        } else {
            break;
            //return expr;
        }
    }

    if(est != NULL)
        est->expression_type = t;

    return expr;
}

void parse_expression_statement(Parser * par, Statement * smt) {
    smt->st = malloc(sizeof(ExpressionStatement));
    ((ExpressionStatement *)smt->st)->token = par->current_token;
    /*((ExpressionStatement *)smt->st)->expression_type =
        par->current_token.type;*/
    ((ExpressionStatement *)smt->st)->expression =
        parse_expression(par, LOWEST, ((ExpressionStatement *)smt->st));

    if(peek_token_is(par, SEMICOLON))
        parser_next_token(par);
}

void parse_return_statement(Parser * par, Statement * smt) {
    smt->st = malloc(sizeof(ReturnStatement));
    ((ReturnStatement *)smt->st)->token = par->current_token;

    parser_next_token(par);

    while(!cur_token_is(par, SEMICOLON))
        parser_next_token(par);
}

Program * parse_program(Parser * parser) {
    size_t sz;
    Program * prg = malloc(sizeof(Program));
    prg->statements = malloc(sizeof(Statement));
    prg->sc = 0;

    while(strcmp(parser->current_token.type, EOF_) != 0) {
        if(strcmp(parser->current_token.type, LET) == 0) {
            sz = sizeof(Statement) * (prg->sc+1);
            prg->statements = realloc(prg->statements, sz);
            prg->statements[prg->sc].type = LET;
            parse_let_statement(parser, &prg->statements[prg->sc++]);
        } else if(strcmp(parser->current_token.type, RETURN) == 0) {
            sz = sizeof(Statement) * (prg->sc+1);
            prg->statements = realloc(prg->statements, sz);
            prg->statements[prg->sc].type = RETURN;
            parse_return_statement(parser, &prg->statements[prg->sc++]);
        } else {
            sz = sizeof(Statement) * (prg->sc+1);
            prg->statements = realloc(prg->statements, sz);
            prg->statements[prg->sc].type = "EXPRESSION";
            parse_expression_statement(parser, &prg->statements[prg->sc++]);
        }

        parser_next_token(parser);
    }

    return prg;
}

bool check_parser_errors(Parser * parser) {
    int i;

    if(parser->ec == 0)
        return false;

    if(parser->ec != 0)
        for(i = 0; i < parser->ec; i++)
            printf("%s\n", parser->errors[i]);

    return true;
}

char * print_let_statement(LetStatement * smt) {
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

char * print_return_statement(ReturnStatement * smt) {
    int v = (strlen(smt->token.literal) + 2);

    char * let = malloc(sizeof(char) * v);

    let[0] = '\0';

    strcat(let, smt->token.literal);
    strcat(let, ";");

    return let;
}

char * print_integer_literal(IntegerLiteral * il) {
    return il->token.literal;
}

char * print_identifier_value(Identifier * i) {
    return i->value;
}

char * print_prefix_expression(PrefixExpression * pe);
char * print_infix_expression(InfixExpression * ie);

char * get_print_expression(char * type, void * expr) {
    char * s;

    if(strcmp(type, INT) == 0)
        s = print_integer_literal((IntegerLiteral *) expr);
    else if (strcmp(type, IDENT) == 0)
        s = print_identifier_value((Identifier *) expr);
    else if (strcmp(type, "PREFIX") == 0)
        s = print_prefix_expression((PrefixExpression *) expr);
    else if (strcmp(type, "INFIX") == 0)
        s = print_infix_expression((InfixExpression *) expr);

    return s;
}

char * print_prefix_expression(PrefixExpression * pe) {
    int v = (strlen(pe->operator) + 2);

    char * exp = malloc(v);
    char * s;

    exp[0] = '\0';

    strcat(exp, "(");
    strcat(exp, pe->operator);

    s = get_print_expression(pe->expression_type, pe->right);
    exp = realloc(exp, 1 + v + strlen(s));

    strcat(exp, s);
    strcat(exp, ")");

    return exp;
}

char * print_infix_expression(InfixExpression * ie) {
    char * left = get_print_expression(ie->left_expression_type, ie->left);
    char * right = get_print_expression(ie->right_expression_type, ie->right);
    char * exp = malloc(1 + (strlen(ie->operator) + 5) + strlen(left) +
        strlen(right));

    exp[0] = '\0';

    strcat(exp, "(");
    strcat(exp, left);
    strcat(exp, " ");
    strcat(exp, ie->operator);
    strcat(exp, " ");
    strcat(exp, right);
    strcat(exp, ")");

    return exp;
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

    Lexer * lexer = new_lexer(input);
    Token token = lexer_next_token(lexer);

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

void test_let_statements() {
    int i;

    const char * input = " \
            let x = 5; \
            let y = 10; \
            let foobar = 838383;";

    const char * tests[3] = {"x", "y", "foobar"};

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Statement stmt;
    LetStatement * ls;
    const char * name;

    if(check_parser_errors(parser))
        return;

    if(program == NULL)
        printf("parse_program() returned NULL");

    if(program->sc != 3)
        printf("Expected 3 statements, got %i", program->sc);

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        stmt = program->statements[i];
        name = tests[i];
        ls = (LetStatement *) stmt.st;

        if(strcmp(stmt.type, LET) != 0)
            printf("Statement's type not \"LET\", got \"%s\"\n", stmt.type);

        if(strcmp(ls->token.type, LET) != 0)
            printf("Token's type not \"LET\", got \"%s\"\n", ls->token.type);

        if(strcmp(ls->name.value, name) != 0)
            printf("Name's value not '%s', got '%s'\n", name, ls->name.value);

        if(strcmp(ls->name.token.literal, name) != 0)
            printf("Name's token literal not '%s', got '%s'\n", name,
                ls->name.token.literal);
    }
}

void test_return_statements() {
    int i;

    const char * input = " \
            return 5; \
            return 10; \
            return 993322;";

    const char * tests[3] = {"x", "y", "foobar"};

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Statement stmt;
    ReturnStatement * rst;

    if(check_parser_errors(parser))
        return;

    if(program->sc != 3)
        printf("Expected 3 statements, got %i", program->sc);

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        stmt = program->statements[i];
        rst = (ReturnStatement *) stmt.st;

        if(strcmp(rst->token.literal, "return") != 0)
            printf("Token literal not \"RETURN\", got \"%s\"\n",
                rst->token.literal);

        if(strcmp(stmt.type, RETURN) != 0)
            printf("Statement's type not \"RETURN\", got \"%s\"\n", stmt.type);
    }
}

void test_identifier_expression() {
    const char * input = "foobar;";

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Statement stmt = program->statements[0];
    ExpressionStatement * est = (ExpressionStatement *) stmt.st;
    Identifier * id = (Identifier *) est->expression;

    if(check_parser_errors(parser))
        return;

    if(program->sc != 1)
        printf("Expected 1 statements, got %i", program->sc);

    if(strcmp(stmt.type, "EXPRESSION") != 0)
        printf("Stmt's type not \"EXPRESSION\", got \"%s\"\n", stmt.type);

    if(strcmp(est->expression_type, "IDENT") != 0)
        printf("Exp type not \"IDENT\", got \"%s\"\n", est->expression_type);

    if(strcmp(est->token.literal, "foobar") != 0)
        printf("Literal not \"foobar\", got \"%s\"\n", est->token.literal);
}

void test_integer_literal_expression() {
    const char * input = "5;";

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Statement stmt = program->statements[0];
    ExpressionStatement * est = (ExpressionStatement *) stmt.st;
    IntegerLiteral * il = (IntegerLiteral *) est->expression;

    if(check_parser_errors(parser))
        return;

    if(program->sc != 1)
        printf("Expected 1 statements, got %i", program->sc);

    if(il->value != 5)
        printf("Integer literal's value not 5, got %i\n", il->value);

    if(strcmp(stmt.type, "EXPRESSION") != 0)
        printf("Statement's type not \"EXPRESSION\", got \"%s\"\n", stmt.type);
}

void * test_parsing_prefix_expressions() {
    int i;

    struct {
        char * input;
        char * operator;
        int v;
    } tests[2] = {{"!5", "!", 5}, {"-15", "-", 15}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * est;
    PrefixExpression * pex;
    IntegerLiteral * il;

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        lexer = new_lexer(tests[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        est = (ExpressionStatement *) stmt.st;
        pex = (PrefixExpression *) est->expression;
        il = (IntegerLiteral *) pex->right;

        if(check_parser_errors(parser))
            break;

        printf("%s\n", print_prefix_expression(pex));

        if(program->sc != 1)
            printf("Expected 1 statements, got %i", program->sc);

        if(strcmp(stmt.type, "EXPRESSION") != 0)
            printf("st type not \"EXPRESSION\", got \"%s\"\n", stmt.type);

        if(il->value != tests[i].v) {
            printf("Got %i, expected %i \n", il->value, tests[i].v);
        }

        /*printf("Statement count: %i\n", program->sc);
        printf("Statement type: %s\n", stmt.type);
        printf("Expression statement type: %s\n", est->expression_type);
        printf("Prefix expression type: %s\n", pex->expression_type);
        printf("Integer literal value: %i\n\n", il->value);*/
    }
}

void * test_parsing_infix_expressions() {
    int i;

    struct {
        char * input;
        int left_val;
        char * operator;
        int right_val;
    } tests[8] = {{"5 + 5;", 5, "+", 5}, {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5}, {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5}, {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5}, {"5 != 5;", 5, "!=", 5}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * est;
    InfixExpression * iex;
    IntegerLiteral * right_il;
    IntegerLiteral * left_il;

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        lexer = new_lexer(tests[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        est = (ExpressionStatement *) stmt.st;
        iex = (InfixExpression *) est->expression;
        right_il = (IntegerLiteral *) iex->right;
        left_il = (IntegerLiteral *) iex->left;

        if(check_parser_errors(parser))
            break;

        printf("%s\n", print_infix_expression(iex));

        if(program->sc != 1)
            printf("[%i] Expected 1 statements, got %i\n", i, program->sc);

        if(strcmp(iex->operator, tests[i].operator) != 0)
            printf("[%i] Expected operator %s, got %s\n", i, tests[i].operator,
                iex->operator);

        if(left_il->value != tests[i].left_val)
            printf("[%i] Expected left value %i, got %i\n", i,
                tests[i].left_val, left_il->value);

        if(right_il->value != tests[i].right_val)
            printf("[%i] Expected right value %i, got %i\n", i,
                tests[i].right_val, right_il->value);
    }
}

int main(int argc, char * argv[])
{
    if(strcmp(argv[1], "tppe") == 0)
        test_parsing_prefix_expressions();
    else if(strcmp(argv[1], "tpie") == 0)
        test_parsing_infix_expressions();
    else if(strcmp(argv[1], "tile") == 0)
        test_integer_literal_expression();
    else if(strcmp(argv[1], "tie") == 0)
        test_identifier_expression();
    else if(strcmp(argv[1], "trs") == 0)
        test_return_statements();
    else if(strcmp(argv[1], "tnt") == 0)
        test_next_token();
    else if(strcmp(argv[1], "tls") == 0)
        test_let_statements();
    else if(strcmp(argv[1], "repl") == 0) {
        while(1) {
            char str[120];
            Lexer * lexer;
            Token token;
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
