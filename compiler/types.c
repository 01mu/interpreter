/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * compiler/types.c
 *
 */

typedef unsigned char byte;

typedef enum {
    OP_CONSTANT = 0,
    OP_ADD,
} OPCODES;

typedef struct {
    char * name;
    int * widths;
    int wc;
} Definition;

typedef struct {
    byte * instruction;
    int instruction_length;
} Instruction;

typedef struct {
    Instruction * instructions;
    Object ** constants;
    int constant_count;
} Compiler;

typedef struct {
    Instruction * instructions;
    Object ** constants;
    int constant_count;
} Bytecode;

typedef struct {
    char * input;
    int expected;
} VMTest;

typedef struct {
    int * operands;
    int length;
    int offset;
} Operands;

HashMap * definitions;

Instruction * make(byte op, int operands[]);
