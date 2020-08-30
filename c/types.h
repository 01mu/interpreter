/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 */

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
