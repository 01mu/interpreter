/*
 *
 * interpreter
 * github.com/01mu/interpreter
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

#define PREC_PEEK 0
#define PREC_CURRENT 1

#define PE_EXPRESSION 0
#define PE_PREFIX 1
#define PE_INFIX 2
#define PE_CONDITION 3
