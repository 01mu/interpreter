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

Bytecode * compiler_get_bytecode(Compiler * c) {
    Bytecode * bc = malloc(sizeof(Bytecode));

    bc->instructions = c->instructions;
    bc->constants = c->constants;
    bc->constant_count = c->constant_count;

    return bc;
}

void * compile(Program * program) {
    return NULL;
}

HashMap * definitions;

void init_definitions() {
    definitions = hash_map_new(60);
}

void free_definitions() {
    hash_map_free(definitions);
}

byte new_op() {
    return op_count++;
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

Instruction * make(byte op, int operands[]) {
    SortedList * sl = hash_map_find(definitions, op_code_to_str(op));

    if(sl == NULL) {
        Instruction * inst = malloc(sizeof(Instruction));
        inst->instruction = NULL;
        inst->instruction_length = 0;

        return inst;
    }

    int oc = sizeof(*operands)/sizeof(operands[0]);
    int instruction_len = 1;

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

    //printf("%i\n",  inst->instruction_length);

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

// =============================================================================
// test_integer_arithmetic
// =============================================================================

void concat_instructions(byte * concat, Instruction ** ei, int ic) {
    int i, j, b;

    for(i = 0, b = 0; i < ic; i++) {
        for(j = 0; j < ei[i]->instruction_length; j++, b++) {
            concat[b] = ei[i]->instruction[j];
        }
    }
}

bool comp_test_integer_object(Object * obj, int test) {
    IntegerObject * v = obj->value;
    return v->value == test;
}

void test_constants(int * ec, int cc, Object ** actual, int ac) {
    int i;

    for(i = 0; i < cc; i++) {
        printf("%i %i\n", ec[i], comp_test_integer_object(actual[i], ec[i]));
    }
}

void test_instructions(Instruction ** ei, int ic, Instruction * instructions) {
    int i, sz = 0;
    byte * concat = NULL;

    for(i = 0; i < ic; i++) {
        sz += ei[i]->instruction_length;
    }

    concat = malloc(sizeof(byte) * sz);
    concat_instructions(concat, ei, ic);

    //printf("%i %i\n", sz, instructions->instruction_length);
}

void run_compiler_tests(CompilerTest ** tests, int tc, int z[]) {
    int i;
    Program * program = NULL;
    Compiler * compiler = NULL;
    Bytecode * bytecode = NULL;

    for(i = 0; i < tc; i++) {
        program = comp_test_parse(tests[i]->input);
        compiler = compiler_new();

        if(compile(program) != NULL) {
            break;
        }

        bytecode = compiler_get_bytecode(compiler);

        test_instructions(tests[i]->expected_instructions, tests[i]->ic,
            bytecode->instructions);

        break;

        test_constants(tests[i]->expected_constants, tests[i]->cc,
            bytecode->constants, bytecode->constant_count);
    }
}

void test_integer_arithmetic() {
    CompilerTest ** tests = malloc(sizeof(CompilerTest *));

    CompilerTest * test = malloc(sizeof(CompilerTest));
    //int * a = malloc(sizeof(int)), * b = malloc(sizeof(int));

    test->input = malloc(sizeof(char) * 10);
    sprintf(test->input, "%s", "1 + 2");

    test->expected_constants = malloc(sizeof(int) * 2);
    test->expected_constants[0] = 1;
    test->expected_constants[1] = 2;
    test->expected_instructions = malloc(sizeof(byte *) * 2);

    int a[1] = {0}, b[1] = {1};
    test->expected_instructions[0] = make(0, a);
    test->expected_instructions[1] = make(0, b);

    test->ic = 2;
    test->cc = 2;

    tests[0] = test;

    int v[2] = {1,2};

    run_compiler_tests(tests, 1, v);
}

// =============================================================================
// test_instruction_string
// =============================================================================

void test_instruction_string() {
    Instruction ** instructions = malloc(sizeof(Instruction *) * 3);
    int a[1] = {1}, b[1] = {2}, c[1] = {65535};

    instructions[0] = make(0, a);
    instructions[1] = make(1, b);
    instructions[2] = make(2, c);

    byte * concat = NULL;
    int i, sz = 0;

    for(i = 0; i < 3; i++) {
        sz += instructions[i]->instruction_length;
    }


    //concat = malloc(sizeof(byte) * sz);

    //concat_instructions(concat, instructions, 3);

}

// =============================================================================
// test_read_operands
// =============================================================================

void read_operands(Definition * def, Instruction * ins) {
    int i;
    int offset = 1;

    for(i = 0; i < def->wc; i++) {
        int width = def->widths[i];

        switch(width) {
            case 2:
            for(int j = 0; j < ins->instruction_length; j++) {
                printf("%i\n", ins->instruction[j]);
            }

            //case 2: encode_big_endian(operands[i], instruction, offset); break;
        }

        offset += width;
    }
}


void test_read_operands() {
    int a[1] = {65534};
    int c = 1, i;
    struct {
        byte opcode;
        int * operands;
        int bytes_read;
    } t[1] = {0, a, 2};

    for(i = 0; i < c; i++) {
        Instruction * instruction = make(t[i].opcode, t[i].operands);
        Definition * def = lookup_op(t[i].opcode);

        read_operands(def, instruction);
    }
}


// =============================================================================

// =============================================================================

void comp_test() {
    init_definitions();
    create_new_definition();
    create_new_definition();
    create_new_definition();
    SortedList * m = hash_map_find(definitions, "0");
    //printf("%p\n", m);
    byte n = 0;

    int op[1] = {65534};
    //Instruction * new_inst = make(n, op);
    //printf("%li\n", sizeof(new_inst) / sizeof(new_inst[0]));
    //printf("%i\n", new_inst->instruction[1]);

    //test_integer_arithmetic();
    //test_instruction_string();
    test_read_operands();
}
