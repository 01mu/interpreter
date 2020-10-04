/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * types/eval.c
 *
 */

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
    char * fn;
} BuiltIn;

typedef struct {
    Identifier ** parameters;
    int pc;
    BlockStatement * body;
    struct Env * env;
} Function;

typedef struct {
    String * value;
} StringObject;

typedef struct {
    Array * elements;
} ArrayObject;

typedef struct Env Env;
