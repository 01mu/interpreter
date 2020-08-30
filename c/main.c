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

#define PRE_LOWEST 1
#define PRE_EQUALS 2
#define PRE_LESSGREATER 3
#define PRE_SUM 4
#define PRE_PRODUCT 5
#define PRE_PREFIX 6
#define PRE_CALL 7

#define EXPRESSION "EXPRESSION"
#define INFIX "INFIX"
#define PREFIX "PREFIX"
#define CALL "CALL"

#define PREC_PEEK 0
#define PREC_CURRENT 1

#define PE_EXPRESSION 0
#define PE_PREFIX 1
#define PE_INFIX 2
#define PE_CONDITION 3

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

typedef struct Statement_ {
    char * type;
    void * st;
} Statement;

typedef struct BlockStatement_ {
    Token token;
    Statement * statements;
    int sc;
} BlockStatement;

typedef struct IfExpression {
    Token token;
    void * condition;
    char * condition_type;
    BlockStatement * consequence;
    BlockStatement * alternative;
} IfExpression;

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

typedef struct Boolean_ {
    Token token;
    bool value;
} Boolean;

typedef struct IntegerLiteral_ {
    Token token;
    int value;
} IntegerLiteral;

typedef struct Identifier_ {
    Token token;
    char * value;
} Identifier;

typedef struct FunctionLiteral_ {
    Token token;
    Identifier ** parameters;
    BlockStatement * body;
    int pc;
} FunctionLiteral;

typedef struct ExpressionStatement_ {
    Token token;
    char * expression_type;
    void * expression;
} ExpressionStatement;

typedef struct CallExpression_ {
    Token token;
    char * function_type;
    void * function;
    ExpressionStatement ** arguments;
    int ac;
} CallExpression;

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

Token new_token(char * type);
void read_char(Lexer * lexer);
int str_to_int(char * str);
Lexer * new_lexer(const char * input);
char * get_substr(int position, int final_pos, const char * input);
bool is_letter(char ch);
bool is_digit(char ch);
char * read_identifier(Lexer * lexer);
char * read_number(Lexer * lexer);
char * lookup_ident(char * literal);
void skip_whitespace(Lexer * lexer);
char peek_char(Lexer * lexer);
Token lexer_next_token(Lexer * lexer);
void parser_next_token(Parser * parser);
Parser * new_parser(Lexer * lexer);
bool cur_token_is(Parser * parser, char * type);
bool peek_token_is(Parser * parser, char * type);

void peek_error(Parser * parser, char * type);
bool expect_peek(Parser * parser, char * type);
bool check_parser_errors(Parser * parser);

void * parse_call_expression(Parser * par, char * type, void * function);
ExpressionStatement ** parse_call_arguments(Parser * par, int * ac);
void * parse_expression(Parser * par, int precedence, void * ex, int et);
void parse_let_statement(Parser * par, Statement * smt);
void parse_return_statement(Parser * par, Statement * smt);
void * parse_identifier(Parser * par);
void * parse_integer_literal(Parser * par);
int parser_get_precedence(Parser * par, int type);
void * parse_prefix_expression(Parser * par);
void * parse_infix_expression(Parser * par, void * left, char * left_type);
void * parse_boolean(Parser * par, bool value);
void * parse_grouped_expression(Parser * par);
void parse_expression_statement(Parser * par, Statement * smt);
void * parse_if_expression(Parser * par);
BlockStatement * parse_block_statement(Parser * parser);
Program * parse_program(Parser * parser);
Identifier ** parse_program_parameters(Parser * parser, int * c);
void * parse_function_literal(Parser * parser);

char * print_function_literal(FunctionLiteral * fl);
char * print_block_statement(BlockStatement * bst);
char * print_if_expression(IfExpression * iex);
char * get_print_expression(char * type, void * expr);
char * print_let_statement(LetStatement * smt);
char * print_return_statement(ReturnStatement * smt);
char * print_integer_literal(IntegerLiteral * il);
char * print_boolean(Boolean * b);
char * print_identifier_value(Identifier * i);
char * print_prefix_expression(PrefixExpression * pe);
char * print_infix_expression(InfixExpression * ie);
char * get_print_statement_type(Statement stmt);
char * print_program(Program * program);

void test_next_token();
void test_let_statements();
void test_return_statements();
void test_identifier_expression();
void test_integer_literal_expression();
void test_parsing_prefix_expressions();
void test_parsing_infix_expressions();
void test_print_program();

Token new_token(char * type) {
    Token token;

    token.type = type;
    token.literal = type;

    return token;
}

void read_char(Lexer * lexer) {
    if(lexer->read_pos >= strlen(lexer->input)) {
        lexer->ch = 0;
    } else {
        lexer->ch = lexer->input[lexer->read_pos];
    }

    lexer->pos = lexer->read_pos;
    lexer->read_pos += 1;
}

int str_to_int(char * s) {
    int r = 0;
    int p = 1;

    while(* s == '-' || * s == '+') {
        if(* s++ == '-') {
            p *= -1;
        }
    }

    while(* s >= '0' && * s <= '9') {
        r = (r * 10) + (* s++ - '0');
    }

    return r * p;
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
    int size = final_pos - position;

    char * buff = malloc(size + 1);

    for(i = 0; position < final_pos; position++, i++) {
        buff[i] = input[position];
    }

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

    while(is_letter(lexer->ch)) {
        read_char(lexer);
    }

    return get_substr(position, lexer->pos, lexer->input);
}

char * read_number(Lexer * lexer) {
    int position = lexer->pos;

    while(is_digit(lexer->ch)) {
        read_char(lexer);
    }

    return get_substr(position, lexer->pos, lexer->input);
}

char * lookup_ident(char * literal) {
    if(strcmp("fn", literal) == 0) {
        return FUNCTION;
    } else if(strcmp("let", literal) == 0) {
        return LET;
    } else if(strcmp("true", literal) == 0) {
        return TRUE;
    } else if(strcmp("false", literal) == 0) {
        return FALSE;
    } else if(strcmp("if", literal) == 0) {
        return IF;
    } else if(strcmp("else", literal) == 0) {
        return ELSE;
    } else if(strcmp("return", literal) == 0) {
        return RETURN;
    } else {
        return IDENT;
    }
}

void skip_whitespace(Lexer * lexer) {
    while(lexer->ch == ' ' || lexer-> ch == '\t' || lexer->ch == '\n' ||
        lexer->ch  == '\r') {

        read_char(lexer);
    }
}

char peek_char(Lexer * lexer) {
    if(lexer->read_pos >= strlen(lexer->input)) {
        return 0;
    } else {
        return lexer->input[lexer->read_pos];
    }
}

Token lexer_next_token(Lexer * lexer) {
    Token token;

    skip_whitespace(lexer);

    switch(lexer->ch) {
    case ';':
        token = new_token(SEMICOLON);
        break;
    case '(':
        token = new_token(LPAREN);
        break;
    case ')':
        token = new_token(RPAREN);
        break;
    case ',':
        token = new_token(COMMA);
        break;
    case '+':
        token = new_token(PLUS);
        break;
    case '{':
        token = new_token(LBRACE);
        break;
    case '}':
        token = new_token(RBRACE);
        break;
    case '-':
        token = new_token(MINUS);
        break;
    case '*':
        token = new_token(ASTERISK);
        break;
    case '/':
        token = new_token(SLASH);
        break;
    case '<':
        token = new_token(LT);
        break;
    case '>':
        token = new_token(GT);
        break;
    case '=':
        if(peek_char(lexer) == '=') {
            read_char(lexer);
            token.type = EQ;
            token.literal = "==";
        } else {
            token = new_token(ASSIGN);
        }
        break;
    case '!':
        if(peek_char(lexer) == '=') {
            read_char(lexer);
            token.type = NOT_EQ;
            token.literal = "!=";
        } else {
            token = new_token(BANG);
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
            token = new_token(ILLEGAL);
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

bool cur_token_is(Parser * parser, char * type) {
    return strcmp(parser->current_token.type, type) == 0;
}

bool peek_token_is(Parser * parser, char * type) {
    return strcmp(parser->peek_token.type, type) == 0;
}

void peek_error(Parser * parser, char * type) {
    int ex_len = strlen(type);
    int gt_len = strlen(parser->peek_token.type);

    char * str = malloc(25 + ex_len + gt_len);

    sprintf(str, "ERROR: Expected %s got %s", type, parser->peek_token.type);

    parser->errors[parser->ec] = str;
    parser->errors[++parser->ec] = malloc(sizeof(char *));
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

bool check_parser_errors(Parser * parser) {
    int i;

    if(parser->ec == 0) {
        return false;
    }

    if(parser->ec != 0) {
        for(i = 0; i < parser->ec; i++) {
            printf("%s\n", parser->errors[i]);
        }
    }

    return true;
}

void * parse_call_expression(Parser * par, char * type, void * function) {
    CallExpression * call = malloc(sizeof(CallExpression));

    call->token = par->current_token;
    call->function_type = type;
    call->function = function;
    call->arguments = parse_call_arguments(par, &call->ac);

    return call;
}

ExpressionStatement ** parse_call_arguments(Parser * par, int * ac) {
    ExpressionStatement ** args = malloc(sizeof(ExpressionStatement *));
    int sz = 0;

    ExpressionStatement * e;

    if(peek_token_is(par, RPAREN)) {
        * ac = sz;
        parser_next_token(par);
        return args;
    }

    parser_next_token(par);

    args = realloc(args, sizeof(ExpressionStatement *) * (sz + 1));
    args[sz] = malloc(sizeof(ExpressionStatement));
    e = (ExpressionStatement *) args[sz++];
    e->expression = parse_expression(par, PRE_LOWEST, e, PE_EXPRESSION);

    while(peek_token_is(par, COMMA)) {
        parser_next_token(par);
        parser_next_token(par);

        args = realloc(args, sizeof(ExpressionStatement *) * (sz + 1));
        args[sz] = malloc(sizeof(ExpressionStatement));
        e = (ExpressionStatement *) args[sz++];
        e->expression = parse_expression(par, PRE_LOWEST, e, PE_EXPRESSION);
    }

    if(!expect_peek(par, RPAREN)) {
        * ac = sz;
        return NULL;
    }

    * ac = sz;

    return args;
}

void * parse_expression(Parser * par, int precedence, void * ex, int et) {
    char * type = par->current_token.type;

    char * exp_type;
    void * expr;

    int type_len;

    if(type == IDENT) {
        exp_type = IDENT;
        expr = parse_identifier(par);
    } else if(type == INT) {
        exp_type = INT;
        expr = parse_integer_literal(par);
    } else if(type == TRUE) {
        exp_type = TRUE;
        expr = parse_boolean(par, true);
    } else if(type == FALSE) {
        exp_type = FALSE;
        expr = parse_boolean(par, false);
    } else if(type == BANG || type == MINUS) {
        exp_type = PREFIX;
        expr = parse_prefix_expression(par);
    } else if(type == LPAREN) {
        exp_type = INFIX;
        expr = parse_grouped_expression(par);
    } else if(type == IF) {
        exp_type = IF;
        expr = parse_if_expression(par);
    } else if(type == FUNCTION) {
        exp_type = FUNCTION;
        expr = parse_function_literal(par);
    }

    while(!peek_token_is(par, SEMICOLON) &&
        precedence < parser_get_precedence(par, PREC_PEEK)) {

        type = par->peek_token.type;
        parser_next_token(par);

        if(type == PLUS || type == MINUS || type == SLASH ||
            type == ASTERISK || type == EQ || type == NOT_EQ ||
            type == LT || type == GT) {

            expr = parse_infix_expression(par, expr, exp_type);
            exp_type = INFIX;
        } else if(type == LPAREN) {
            expr = parse_call_expression(par, exp_type, expr);
            exp_type = CALL;
        }
    }

    if(ex != NULL) {
        type_len = strlen(exp_type) + 1;

        if(et == PE_EXPRESSION) {
            ((ExpressionStatement *) ex)->expression_type = malloc(type_len);
            strcpy(((ExpressionStatement *) ex)->expression_type, exp_type);
        } else if(et == PE_PREFIX){
            ((PrefixExpression *) ex)->expression_type = malloc(type_len);
            strcpy(((PrefixExpression *) ex)->expression_type, exp_type);
        } else if(et == PE_INFIX){
            ((InfixExpression *) ex)->right_expression_type = malloc(type_len);
            strcpy(((InfixExpression *) ex)->right_expression_type, exp_type);
        } else if(et == PE_CONDITION){
            ((IfExpression *) ex)->condition_type = malloc(type_len);
            strcpy(((IfExpression *) ex)->condition_type, exp_type);
        }
    }

    return expr;
}

void parse_let_statement(Parser * par, Statement * smt) {
    LetStatement * let;
    ExpressionStatement * ex;

    smt->st = malloc(sizeof(LetStatement));
    let = (LetStatement *) smt->st;
    let->token = par->current_token;

    if(!expect_peek(par, IDENT)) {
        return;
    }

    let->name.token = par->current_token;
    let->name.value = par->current_token.literal;

    if(!expect_peek(par, ASSIGN)) {
        return;
    }

    parser_next_token(par);

    let->value = malloc(sizeof(ExpressionStatement));
    ex = (ExpressionStatement *) let->value;
    ex->expression = parse_expression(par, PRE_LOWEST, ex, PE_EXPRESSION);

    let->type = ex->expression_type;

    while(!cur_token_is(par, SEMICOLON)) {
        parser_next_token(par);
    }
}

void parse_return_statement(Parser * par, Statement * smt) {
    ReturnStatement * ret;
    ExpressionStatement  * ex;

    smt->st = malloc(sizeof(ReturnStatement));
    ret = (ReturnStatement *) smt->st;

    ret->token = par->current_token;

    parser_next_token(par);

    ret->value = malloc(sizeof(ExpressionStatement));
    ex = (ExpressionStatement *) ret->value;
    ex->expression = parse_expression(par, PRE_LOWEST, ex, PE_EXPRESSION);

    ret->type = ex->expression_type;

    while(!cur_token_is(par, SEMICOLON)) {
        parser_next_token(par);
    }
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

int parser_get_precedence(Parser * par, int type) {
    char * pt;

    if(type == PREC_PEEK)
        pt = par->peek_token.type;
    else if(type == PREC_CURRENT)
        pt = par->current_token.type;

    if(pt == EQ || pt == NOT_EQ) {
        return PRE_EQUALS;
    } else if(pt == LT || pt == GT) {
        return PRE_LESSGREATER;
    } else if(pt == PLUS || pt == MINUS) {
        return PRE_SUM;
    } else if(pt == SLASH || pt == ASTERISK) {
        return PRE_PRODUCT;
    } else if(pt == LPAREN) {
        return PRE_CALL;
    }

    return PRE_LOWEST;
}

void * parse_prefix_expression(Parser * par) {
    PrefixExpression * pe = malloc(sizeof(PrefixExpression));

    pe->token = par->current_token;
    pe->operator = par->current_token.literal;

    parser_next_token(par);

    pe->right = parse_expression(par, PRE_PREFIX, pe, PE_PREFIX);

    return pe;
}

void * parse_infix_expression(Parser * par, void * left, char * left_type) {
    InfixExpression * ie = malloc(sizeof(InfixExpression));
    int precedence = parser_get_precedence(par, PREC_CURRENT);

    ie->token = par->current_token;
    ie->operator = par->current_token.literal;
    ie->left_expression_type = left_type;
    ie->left = left;

    parser_next_token(par);

    ie->right = parse_expression(par, precedence, ie, PE_INFIX);

    return ie;
}

void * parse_boolean(Parser * par, bool value) {
    Boolean * bool_ = malloc(sizeof(Boolean));

    bool_->token = par->current_token;
    bool_->value = value;

    return bool_;
}

void * parse_grouped_expression(Parser * par) {
    void * s;

    parser_next_token(par);

    s = parse_expression(par, PRE_LOWEST, NULL, 0);

    if(!expect_peek(par, RPAREN)) {
        free(s);
        return NULL;
    }

    return s;
}

void parse_expression_statement(Parser * par, Statement * smt) {
    ExpressionStatement * est;

    smt->st = malloc(sizeof(ExpressionStatement));
    est = (ExpressionStatement *) smt->st;

    est->token = par->current_token;
    est->expression = parse_expression(par, PRE_LOWEST, est, PE_EXPRESSION);

    if(peek_token_is(par, SEMICOLON)) {
        parser_next_token(par);
    }
}

void parse_statement(Parser * par, Statement * stmts, int sc, int sz) {
    char * token_type = par->current_token.type;

    if(strcmp(token_type, LET) == 0) {
        stmts[sc].type = LET;
        parse_let_statement(par, &stmts[sc]);
    } else if(strcmp(token_type, RETURN) == 0) {
        stmts[sc].type = RETURN;
        parse_return_statement(par, &stmts[sc]);
    } else {
        stmts[sc].type = EXPRESSION;
        parse_expression_statement(par, &stmts[sc]);
    }
}

void * parse_if_expression(Parser * par) {
    IfExpression * iex = malloc(sizeof(IfExpression));
    iex->token = par->current_token;

    if(!expect_peek(par, LPAREN)) {
        return NULL;
    }

    parser_next_token(par);

    iex->condition = parse_expression(par, PRE_LOWEST, iex, PE_CONDITION);

    if(!expect_peek(par, RPAREN)) {
        return NULL;
    }

    if(!expect_peek(par, LBRACE)) {
        return NULL;
    }

    iex->consequence = parse_block_statement(par);

    if(peek_token_is(par, ELSE)) {
        parser_next_token(par);

        if(!expect_peek(par, LBRACE)) {
            return NULL;
        }

        iex->alternative = parse_block_statement(par);
    }

    return iex;
}

BlockStatement * parse_block_statement(Parser * parser) {
    BlockStatement * bs = malloc(sizeof(BlockStatement));
    int sz = 0;

    bs->statements = malloc(sizeof(Statement));
    bs->sc = 0;

    parser_next_token(parser);

    while(!cur_token_is(parser, RBRACE) && !cur_token_is(parser, EOF_)) {
        sz = sizeof(Statement) * (bs->sc + 1);
        bs->statements = realloc(bs->statements, sz);

        parse_statement(parser, bs->statements, bs->sc++, sz);
        parser_next_token(parser);
    }

    return bs;
}

Program * parse_program(Parser * parser) {
    Program * prg = malloc(sizeof(Program));
    int sz = 0;

    prg->statements = malloc(sizeof(Statement));
    prg->sc = 0;

    while(!cur_token_is(parser, EOF_)) {
        sz = sizeof(Statement) * (prg->sc + 1);
        prg->statements = realloc(prg->statements, sz);

        parse_statement(parser, prg->statements, prg->sc++, sz);
        parser_next_token(parser);
    }

    return prg;
}

Identifier ** parse_program_parameters(Parser * parser, int * c) {
    Identifier ** parameters = malloc(sizeof(Identifier *));
    int pc = 0;

    if(peek_token_is(parser, RPAREN)) {
        * c = pc;
        parser_next_token(parser);
        return parameters;
    }

    parser_next_token(parser);

    Identifier * parameter = malloc(sizeof(Identifier));

    parameter->token = parser->current_token;
    parameter->value = parser->current_token.literal;

    parameters[pc++] = parameter;
    parameters = realloc(parameters, sizeof(Identifier *) * (pc + 1));

    while(peek_token_is(parser, COMMA)) {
        parser_next_token(parser);
        parser_next_token(parser);

        Identifier * parameter = malloc(sizeof(Identifier));

        parameter->token = parser->current_token;
        parameter->value = parser->current_token.literal;

        parameters[pc++] = parameter;
        parameters = realloc(parameters, sizeof(Identifier *) * (pc + 1));
    }

    if(!expect_peek(parser, RPAREN)) {
        * c = pc;
        return NULL;
    }

    * c = pc;

    return parameters;
}

void * parse_function_literal(Parser * parser) {
    FunctionLiteral * fl = malloc(sizeof(FunctionLiteral));

    fl->token = parser->current_token;

    if(!expect_peek(parser, LPAREN)) {
        return NULL;
    }

    fl->parameters = parse_program_parameters(parser, &fl->pc);

    if(!expect_peek(parser, LBRACE)) {
        return NULL;
    }

    fl->body = parse_block_statement(parser);

    return fl;
}

char * print_call_expression(CallExpression * ce) {
    int i;

    ExpressionStatement * es;

    char * fnc = get_print_expression(ce->function_type, ce->function);
    char * fin = malloc(sizeof(char) * strlen(fnc));
    char * args = malloc(sizeof(char));
    char * exp;

    fin[0] = '\0';
    args[0] = '\0';

    int c = 1;

    for(i = 0; i < ce->ac; i++) {
        es = ce->arguments[i];
        exp = get_print_expression(es->expression_type, es->expression);
        c += strlen(exp) + 2;
        args = realloc(args, c);
        strcat(args, exp);

        if(i != ce->ac - 1) {
            strcat(args, ", ");
        }
    }

    fin = realloc(fin, strlen(args) + strlen(fnc) + 3);

    strcat(fin, fnc);
    strcat(fin, "(");
    strcat(fin, args);
    strcat(fin, ")");

    free(args);

    return fin;
}

char * print_function_literal(FunctionLiteral * fl) {
    int i;

    Identifier * id;

    char * params = malloc(sizeof(char));
    char * fin = malloc(sizeof(char) + 4);
    char * block;

    int psz = 1;

    params[0] = '\0';
    fin[0] = '\0';

    for(i = 0; i < fl->pc; i++) {
        id = fl->parameters[i];

        psz += strlen(id->value) + 2;
        params = realloc(params, psz);
        strcat(params, id->value);

        if(i != fl->pc - 1) {
            strcat(params, ", ");
        }
    }

    block = print_block_statement(fl->body);

    strcat(fin, "fn (");
    fin = realloc(fin, strlen(params) + strlen(block) + 7);
    strcat(fin, params);
    strcat(fin, ") ");
    strcat(fin, block);

    free(params);

    return fin;
}

char * print_block_statement(BlockStatement * bst) {
    int i;

    Statement stmt;

    char * pt = malloc(sizeof(char *));
    char * ap = NULL;

    int len = 4;

    pt[0] = '\0';

    strcat(pt, "{ ");

    for(i = 0; i < bst->sc; i++) {
        ap = get_print_statement_type(bst->statements[i]);

        len += strlen(ap) + 1;
        pt = realloc(pt, len);

        strcat(pt, ap);
        strcat(pt, " ");

        free(ap);
    }

    strcat(pt, "}");

    return pt;
}

char * print_if_expression(IfExpression * iex) {
    char * cond = get_print_expression(iex->condition_type, iex->condition);

    char * cons = print_block_statement(iex->consequence);
    char * alt = NULL;

    char * pt = malloc(5 + strlen(cond) + strlen(cons));

    pt[0] = '\0';

    strcat(pt, "if");
    strcat(pt, " ");
    strcat(pt, cond);
    strcat(pt, " ");
    strcat(pt, cons);

    if(iex->alternative != NULL) {
        alt = print_block_statement(iex->alternative);
        pt = realloc(pt, 6 + strlen(alt) +  strlen(cond) + strlen(cons));

        strcat(pt, " ");
        strcat(pt, alt);
    }

    return pt;
}

char * get_print_expression(char * type, void * expr) {
    char * s;

    if(strcmp(type, INT) == 0) {
        s = print_integer_literal((IntegerLiteral *) expr);
    } else if (strcmp(type, IDENT) == 0) {
        s = print_identifier_value((Identifier *) expr);
    } else if (strcmp(type, PREFIX) == 0) {
        s = print_prefix_expression((PrefixExpression *) expr);
    } else if (strcmp(type, INFIX) == 0) {
        s = print_infix_expression((InfixExpression *) expr);
    } else if (strcmp(type, TRUE) == 0 || strcmp(type, FALSE) == 0) {
        s = print_boolean((Boolean *) expr);
    } else if(strcmp(type, IF) == 0) {
        s = print_if_expression((IfExpression *) expr);
    } else if(strcmp(type, FUNCTION) == 0) {
        s = print_function_literal((FunctionLiteral *) expr);
    } else if(strcmp(type, CALL) == 0) {
        s = print_call_expression((CallExpression *) expr);
    }

    return s;
}

char * print_let_statement(LetStatement * smt) {
    int v = (strlen(smt->token.literal) + strlen(smt->name.value) + 5);

    ExpressionStatement * est = NULL;

    char * ap;
    char * let = malloc(sizeof(char) * v);

    let[0] = '\0';

    strcat(let, smt->token.literal);
    strcat(let, " ");
    strcat(let, smt->name.value);
    strcat(let, " = ");

    if(smt->value != NULL) {
        est = (ExpressionStatement *) smt->value;
        ap = get_print_expression(est->expression_type, est->expression);
        let = realloc(let, sizeof(char) * (v + strlen(ap) + 1));
        strcat(let, ap);
    }

    strcat(let, ";");
    free(ap);

    return let;
}

char * print_return_statement(ReturnStatement * smt) {
    int v = (strlen(smt->token.literal) + 3);

    char * ret = malloc(sizeof(char) * v);
    ExpressionStatement * est = NULL;
    char * ap;

    ret[0] = '\0';

    strcat(ret, smt->token.literal);
    strcat(ret, " ");

    if(smt->value != NULL) {
        est = (ExpressionStatement *) smt->value;
        ap = get_print_expression(smt->type, est->expression);
        ret = realloc(ret, v + strlen(ap));
        strcat(ret, ap);
    }

    strcat(ret, ";");
    free(ap);

    return ret;
}

char * print_integer_literal(IntegerLiteral * il) {
    return il->token.literal;
}

char * print_boolean(Boolean * b) {
    return b->token.literal;
}

char * print_identifier_value(Identifier * i) {
    return i->value;
}

char * print_prefix_expression(PrefixExpression * pe) {
    char * ap = get_print_expression(pe->expression_type, pe->right);
    char * exp = malloc(strlen(ap) + strlen(pe->operator) + 3);

    sprintf(exp, "(%s%s)", pe->operator, ap);

    free(ap);

    return exp;
}

char * print_infix_expression(InfixExpression * ie) {
    char * l = get_print_expression(ie->left_expression_type, ie->left);
    char * r = get_print_expression(ie->right_expression_type, ie->right);
    char * exp = malloc(5 + strlen(ie->operator) + strlen(l) + strlen(r));

    sprintf(exp, "(%s %s %s)", l, ie->operator, r);

    free(l);
    free(r);

    return exp;
}

char * get_print_statement_type(Statement stmt) {
    ExpressionStatement * est = NULL;

    char * ap = malloc(sizeof(char *));
    char * type = NULL;

    ap[0] = '\0';

    if(stmt.type == LET) {
        ap = print_let_statement((LetStatement *) stmt.st);
    } else if(stmt.type == RETURN) {
        ap = print_return_statement((ReturnStatement *) stmt.st);
    } else if(stmt.type == EXPRESSION) {
        type = ((ExpressionStatement *) stmt.st)->expression_type;
        est = ((ExpressionStatement *) stmt.st)->expression;
        ap = get_print_expression(type, est);
    }

    return ap;
}

char * print_program(Program * program) {
    int i;

    char * pt = malloc(sizeof(char));
    char * ap = NULL;

    int len = 1;

    pt[0] = '\0';

    for(i = 0; i < program->sc; i++) {
        ap = get_print_statement_type(program->statements[i]);
        len += strlen(ap) + 1;
        pt = realloc(pt, len);

        strcat(pt, ap);
        strcat(pt, "\n");

        free(ap);
    }

    return pt;
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

void test_parsing_prefix_expressions() {
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
    }
}

void test_parsing_infix_expressions() {
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

void test_parsing_if_expressions() {
    int i;

    struct {
        char * input;
        char * condition_type;;
        char * cons_type;
        char * alt_type;
    } tests[4] = {
            {"if (x) { let a = b; } else { return 1; }", IDENT, LET, RETURN},
            {"if (x) { let a = b; } else { 5 }", IDENT, LET, EXPRESSION},
            {"if (x) { let a = b; } else { if (z) { } else { } }",
                    IDENT, LET, EXPRESSION},
            {"if (a > 3) { !2 } else { 3 }", INFIX, EXPRESSION, EXPRESSION}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    IfExpression * ifx;
    BlockStatement * cons;
    BlockStatement * alt;

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        lexer = new_lexer(tests[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = stmt.st;
        ifx = (IfExpression *) es->expression;
        cons = (BlockStatement *) ifx->consequence;
        alt = (BlockStatement *) ifx->alternative;

        if(check_parser_errors(parser)) {
            break;
        }

        if(strcmp(stmt.type, EXPRESSION) != 0) {
            printf("[%i] Expected statement type IF, got: %s\n", i, stmt.type);
        }

        if(strcmp(ifx->condition_type, tests[i].condition_type) != 0) {
            printf("[%i] Expected condition type %s, got: %s\n",
                i, tests[i].condition_type, ifx->condition_type);
        }

        if(strcmp(cons->statements[0].type, tests[i].cons_type) != 0) {
            printf("[%i] Expected consequence type %s, got: %s\n",
                i, tests[i].cons_type, cons->statements[0].type);
        }

        if(strcmp(alt->statements[0].type, tests[i].alt_type) != 0) {
            printf("[%i] Expected altnerative type %s, got: %s\n",
                i, tests[i].alt_type, alt->statements[0].type);
        }
    }
}

void test_print_program() {
    int i;

    const char * input = " \
        if (x < y) { x } else { !!!!5 }; \
        if (x < y) { a } else { if (x < y) { x } else { !!!!5 }; }; \
        -5; \
        fn(x, y, z) { z }; \
        !!!4; \
        a + b / 4; \
        -(1 + 2); \
        let a = a * 2 + 3; \
        let v = n; \
        3 > 5 == false; \
        false; \
        !true; \
        !!asd; \
        let a = asd(12 + 3, asd, 3);\
        return 500;";

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);

    for(i = 0; i < parser->ec; i++) {
        printf("%s\n", parser->errors[i]);
    }

    printf("%i\n", program->sc);

    printf("%s", print_program(program));
}

int main(int argc, char * argv[])
{
    if(strcmp(argv[1], "ppe") == 0) {
        test_parsing_prefix_expressions();
    } else if(strcmp(argv[1], "if") == 0) {
        test_parsing_if_expressions();
    } else if(strcmp(argv[1], "pie") == 0) {
        test_parsing_infix_expressions();
    } else if(strcmp(argv[1], "ile") == 0) {
        test_integer_literal_expression();
    } else if(strcmp(argv[1], "ie") == 0) {
        test_identifier_expression();
    } else if(strcmp(argv[1], "rs") == 0) {
        test_return_statements();
    } else if(strcmp(argv[1], "nt") == 0) {
        test_next_token();
    } else if(strcmp(argv[1], "ls") == 0) {
        test_let_statements();
    } else if(strcmp(argv[1], "pp") == 0) {
        test_print_program();
    } else if(strcmp(argv[1], "repl") == 0) {
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
