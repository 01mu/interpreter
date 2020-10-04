/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * types/store.c
 *
 */

typedef struct {
    FunctionLiteral ** store;
    int count;
} FunctionLiteralStore;

typedef struct {
    struct Env ** store;
    int count;
} EnvStore;
