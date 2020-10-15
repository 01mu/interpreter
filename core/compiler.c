/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/compiler.c
 *
 */

typedef char Opcode;
typedef char * Instructions;

typedef struct {
    int op;
} OpStore;

typedef struct {
    char * name;
    int * width;
} Definition;

HashMap * definitions;

OpStore * opstore;

void init_opstore() {
    opstore = malloc(sizeof(OpStore));
    opstore->op = 0;
}

char * new_op_code() {

}
