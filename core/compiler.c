/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/compiler.c
 *
 */

typedef unsigned char byte;

byte op_count;

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
    Instruction ** expected_instructions;
    int * expected_constants;
    int cc;
    int ic;
} CompilerTest;


Compiler * compiler_new() {
    Compiler * compiler = malloc(sizeof(Compiler));

    compiler->instructions = malloc(sizeof(Instruction));
    compiler->constants = malloc(sizeof(Object *));
    compiler->constant_count = 0;

    return compiler;
}

Bytecode * bytecode(Compiler * c) {
    Bytecode * bc = malloc(sizeof(Bytecode));

    bc->instructions = c->instructions;
    bc->constants = c->constants;
    bc->constant_count = c->constant_count;

    return bc;
}

void compile(Program * program) {

}

HashMap * definitions;

void init_definitions() {
    definitions = hash_map_new(60);
}

void free_definitions() {
    hash_map_free(definitions);
}

char * new_op_constant() {
    char * z = malloc(20);
    SortedList * find = NULL;

    sprintf(z, "%i", op_count++);

    return z;
}

char * op_code_to_str(byte op) {
    char * z = malloc(20);
    sprintf(z, "%i", op);
    return z;
}

Definition * op_new_definition(char * name, int * widths, int wc) {
    Definition * new_def = malloc(sizeof(Definition));

    new_def->name = name;
    new_def->widths = widths;
    new_def->wc = wc;

    return new_def;
}

byte * encode_big_endian(int num, byte * arr, int offset) {
    int i, c = 0;
    int t[4] = {(num & 0x000000ff) << 24u,
                (num & 0x0000ff00) << 8u,
                (num & 0x00ff0000) >> 8u,
                (num & 0xff000000) >> 24u};

    for(i = 3; i >= 0; i--) {
        if(t[i]) {
            arr = realloc(arr, sizeof(byte) * (c + 1));

            if(__bswap_32(t[i]) >= 255) {
                arr[offset + c++] = 255;
            } else {
                arr[offset + c++] = __bswap_32(t[i]);
            }
        }
    }

    return arr;
}

Instruction * make(byte op, int * operands) {
    SortedList * sl = hash_map_find(definitions, op_code_to_str(op));
    int oc = sizeof(operands)/sizeof(operands[0]);
    int instruction_len = 1;

    if(sl == NULL) {
        return NULL;
    }

    Definition * def = sl->data;

    for(int i = 0; i < def->wc; i++) {
        instruction_len += def->widths[i];
    }

    byte * instruction = malloc(sizeof(byte));
    int c = 1, offset = 1;

    instruction[0] = op;

    for(int i = 0; i < oc; i++) {
        int width = def->widths[i];

        switch(width) {
            case 2: encode_big_endian(operands[i], instruction, offset); break;
        }

        offset += width;
    }

    Instruction * inst = malloc(sizeof(Instruction));
    inst->instruction = instruction;
    inst->instruction_length = offset;

    return inst;
}

void create_new_definition() {
    char * name = malloc(20);
    name[0] = '\0';
    strcpy(name, "OpConstant");

    int * widths = malloc(sizeof(int) * 1);
    widths[0] = 2;

    Definition * new_def = op_new_definition(name, widths, 1);
    hash_map_insert(definitions, new_op_constant(), new_def);
}

Definition * lookup_op(byte op) {
    char * z = op_code_to_str(op);
    SortedList * find = NULL;

    find = hash_map_find(definitions, z);

    return find->data;
}

Program * comp_test_parse(char * input) {
    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);

    return parse_program(parser);
}

Instruction * concat_instructions(Instruction ** expected_instructions, int ic) {
    for(int i = 0; i < ic; i++) {
        for(int j = 0; j < expected_instructions[i]->instruction_length; j++) {
            printf("%i\n", expected_instructions[i]->instruction[j]);
        }
    }
}

void test_instructions(Instruction ** expected_instructions, int ic,
    Instruction * instructions) {

    Instruction * concat = concat_instructions(expected_instructions, ic);


}

void run_compiler_tests(CompilerTest ** tests, int tc, int z[]) {
    int i;

    for(i = 0; i < tc; i++) {
        Program * program = comp_test_parse(tests[i]->input);
        Compiler * compiler = compiler_new();
        Bytecode * bc = bytecode(compiler);

        test_instructions(tests[i]->expected_instructions, tests[i]->ic, bc->instructions);

    }
}

void test_integer_arithmetic() {
    CompilerTest ** tests = malloc(sizeof(CompilerTest *));

    CompilerTest * test = malloc(sizeof(CompilerTest));
    int * a = malloc(sizeof(int)), * b = malloc(sizeof(int));

    test->input = malloc(sizeof(char) * 10);
    sprintf(test->input, "%s", "1 + 2");

    test->expected_constants = malloc(sizeof(int) * 2);
    test->expected_constants[0] = 1;
    test->expected_constants[0] = 2;
    test->expected_instructions = malloc(sizeof(byte *) * 2);

    a[0] = 0;
    b[0] = 1;
    test->expected_instructions[0] = make(0, a);
    test->expected_instructions[1] = make(0, b);

    test->ic = 2;
    test->cc = 2;

    tests[0] = test;

    int v[2] = {1,2};

    run_compiler_tests(tests, 1, v);
}

void comp_test() {
    init_definitions();
    create_new_definition();
    SortedList * m = hash_map_find(definitions, "0");
    printf("%p\n", m);

    byte n = 0;
    printf("%i\n", n);

    int op[1] = {65534};
    Instruction * new_inst = make(n, op);
    //printf("%li\n", sizeof(new_inst) / sizeof(new_inst[0]));
    //printf("%i\n", new_inst->instruction[1]);

    test_integer_arithmetic();
}
