/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * types/parser.c
 *
 */

typedef struct {
    Statement * statements;
    int sc;
} Program;

typedef struct {
    Token token;
    Statement * statements;
    int sc;
} BlockStatement;

typedef struct {
    Token token;
    Token else_token;
    void * condition;
    char * condition_type;
    BlockStatement * consequence;
    BlockStatement * alternative;
} IfExpression;

typedef struct {
    Token token;
    char * operator;
    char * left_expression_type;
    char * right_expression_type;
    void * left;
    void * right;
} InfixExpression;

typedef struct {
    Token token;
    char * operator;
    char * expression_type;
    void * right;
} PrefixExpression;

typedef struct {
    Token token;
    bool value;
} Boolean;

typedef struct {
    Token token;
    int value;
} IntegerLiteral;

typedef struct {
    Token token;
    char * value;
} Identifier;

typedef struct {
    Token token;
    Identifier ** parameters;
    BlockStatement * body;
    int pc;
} FunctionLiteral;

typedef struct {
    Token token;
    char * expression_type;
    void * expression;
} ExpressionStatement;

typedef struct {
    Token token;
    char * function_type;
    void * function;
    ExpressionStatement ** arguments;
    int ac;
} CallExpression;

typedef struct {
    Token token;
    Identifier name;
    char * type;
    void * value;
} LetStatement;

typedef struct {
    Token token;
    char * type;
    void * value;
} ReturnStatement;

typedef struct {
    Token token;
    Array * elements;
} ArrayLiteral;

typedef struct {
    Token token;
    void * left;
    void * index;
    char * left_expression_type;
    char * index_expression_type;
} IndexExpression;

typedef struct {
    Token token;
    String * value;
} StringLiteral;
