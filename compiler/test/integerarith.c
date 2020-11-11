/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * compiler/test/integerairth.c
 *
 */

typedef struct {
    char * input;
    int * expected_constants;
    int ec_count;
    Instruction ** expected_instructions;
    int ei_count;
} CompilerTest;

bool comp_test_integer_object(Object * obj, int test) {
    return ((IntegerObject *) obj->value)->value == test;
}

void test_constants(int ec[], int cc, Object ** actual, int ac) {
    int i;

    if(cc != ac) {
        printf("Invalid constant count\n");
    }

    for(i = 0; i < cc; i++) {
        if(actual[i] != NULL) {
            comp_test_integer_object(actual[i], ec[i]);
        }
    }
}

void test_instructions(Instruction ** ei, int ic, Instruction * ins) {
    int i;
    int conc_sz = 0;
    byte * conc = concat_instructions(ei, ic, &conc_sz);

    if(conc_sz != ins->instruction_length) {
        printf("Invalid size\n");
        return;
    }

    for(i = 0; i < ins->instruction_length; i++) {
        if(conc[i] != ins->instruction[i]) {
            printf("Invalid instruction %i\n", i);
            return;
        }
    }

    free(conc);
}

void free_compiler(Compiler * compiler) {
    int i;

    for(i = 0; i < compiler->constant_count; i++) {
        free(compiler->constants[i]->value);
        free(compiler->constants[i]);
    }

    free_instruction(compiler->instructions);
    free(compiler->constants);
    free(compiler);
}

void run_compiler_tests(CompilerTest ** tests, int tc, int z[]) {
    int i;
    Compiler * compiler = NULL;
    Bytecode * bytecode = NULL;

    for(i = 0; i < tc; i++) {
        Lexer * lexer = new_lexer(tests[i]->input);
        Parser * parser = new_parser(lexer);
        Program * program = parse_program(parser);

        compiler = compiler_new();

        if(compile(compiler, program) != NULL) {
            break;
        }

        bytecode = compiler_get_bytecode(compiler);

        test_instructions(tests[i]->expected_instructions, tests[i]->ei_count,
            bytecode->instructions);

        test_constants(tests[i]->expected_constants, tests[i]->ec_count,
            bytecode->constants, bytecode->constant_count);

        free_program(lexer, parser, program);
        free_compiler(compiler);
        free(bytecode);
    }
}

void test_integer_arithmetic() {
    CompilerTest ** tests = malloc(sizeof(CompilerTest *));
    CompilerTest * test = malloc(sizeof(CompilerTest));
    int ec[2] = {1, 2};
    int a[1] = {0}, b[1] = {1}, c[0] = {};
    Instruction * ei[3] = {make(OP_CONSTANT, a), make(OP_CONSTANT, b),
            make(OP_ADD, c)};

    test->input = malloc(sizeof(char) * 10);
    sprintf(test->input, "%s", "1 + 2");
    test->expected_constants = ec;
    test->ec_count = 2;
    test->expected_instructions = ei;
    test->ei_count = 3;

    tests[0] = test;

    run_compiler_tests(tests, 1, ec);

    for(int i = 0; i < 3; i++) {
        free_instruction(ei[i]);
    }

    for(int i = 0; i < 1; i++) {
        free(tests[i]->input);
        free(tests[i]);
    }

    free(tests);
}
