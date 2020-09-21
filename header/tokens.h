/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * header/tokens.h
 *
 */

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
#define BOOLEAN "BOOLEAN"

#define PREC_PEEK 0
#define PREC_CURRENT 1

#define PE_EXPRESSION 0
#define PE_PREFIX 1
#define PE_INFIX 2
#define PE_CONDITION 3

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define ERROR "ERROR"
