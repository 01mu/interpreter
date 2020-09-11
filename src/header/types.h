/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * header/types.h
 *
 */

struct SortedList {
    struct SortedList * next;
    char * data_type;
    void * data;
    char * key;
};

typedef struct {
    int size;
    struct SortedList ** array;
} HashMap;

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
    char * type;
    void * st;
} Statement;

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
    Statement * statements;
    int sc;
} Program;

typedef struct {
    Lexer * lexer;
    Token current_token;
    Token peek_token;
    char ** errors;
    int ec;
} Parser;

struct Env {
    HashMap * store;
    struct Env * outer;
};

typedef struct {
    char * message;
} ErrorObject;

typedef struct {
    char * type;
    void * value;
} ReturnValue;

typedef struct {

} NullObject;

typedef struct {
    int value;
} IntegerObject;

typedef struct {
    bool value;
} BooleanObject;

typedef struct {
    char * type;
    void * value;
} Object;

typedef struct {
    Identifier ** parameters;
    int pc;
    BlockStatement * body;
    struct Env * env;
} Function;

Object * false_bool = NULL, * true_bool = NULL;
Object * null_obj = NULL;

typedef struct Env Env;
typedef struct SortedList SortedList;
