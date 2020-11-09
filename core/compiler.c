/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/compiler.c
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
    Instruction ** expected_instructions;
    int * expected_constants;
    int cc;
    int ic;
} CompilerTest;

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

int compiler_add_instruction(Compiler * compiler, Instruction * ins) {
    Instruction * comp_instruction = compiler->instructions;
    int pos = comp_instruction->instruction_length;
    int append_len = ins->instruction_length;

    //comp_instruction->instruction_length += new_len;

    comp_instruction->instruction = realloc(comp_instruction->instruction,
        sizeof(byte) * (pos + append_len));

    for(int i = pos, j = 0; i < pos + append_len; i++, j++) {
        comp_instruction->instruction[i] = ins->instruction[j];
        comp_instruction->instruction_length += 1;
    }

    //comp_instruction->instruction_length += (pos + append_len);
}

int compile_emit(Compiler * compiler, byte op, int operands[]) {
    Instruction * ins = make(op, operands);
    return compiler_add_instruction(compiler, ins);
}

int compiler_add_constant(Compiler * compiler, void * constant) {
    int sz = sizeof(Object *) * (compiler->constant_count + 1);

    compiler->constants = realloc(compiler->constants, sz);
    compiler->constants[compiler->constant_count++] = constant;

    return compiler->constant_count - 1;
}

void * compile_expression(Compiler * compiler, char * type, void * expression) {
    //printf("%s\n", type);

    int a[0] = {};

    if(strcmp(type, INFIX) == 0) {
        InfixExpression * iex = (InfixExpression *) expression;
        compile_expression(compiler, iex->left_expression_type, iex->left);
        compile_expression(compiler, iex->right_expression_type, iex->right);

        if(strcmp(iex->operator, "+") == 0) {
            compile_emit(compiler, OP_ADD, a);
        }
    } else if(strcmp(type, INT) == 0) {
        IntegerLiteral * il = (IntegerLiteral *) expression;
        Object * int_obj = eval_integer(il, NULL);
        //printf("%p\n", int_obj);

        //compiler_add_constant(compiler, int_obj);
        //printf("%s\n", compiler->constants[0]->type);
        int v[1] = {compiler_add_constant(compiler, int_obj)};

        compile_emit(compiler, OP_CONSTANT, v);
    }
}

void * compile(Compiler * compiler, Program * program) {
    int i;
    Statement statement;
    LetStatement * let = NULL;
    ReturnStatement * ret = NULL;
    ExpressionStatement * est = NULL;

    for(i = 0; i < program->sc; i++) {
        statement = program->statements[0];

        if(strcmp(statement.type, LET) == 0) {

        } else if(strcmp(statement.type, RETURN) == 0) {

        } else if(strcmp(statement.type, EXPRESSION) == 0) {
            est = (ExpressionStatement *) statement.st;
            compile_expression(compiler, est->expression_type, est->expression);
        }
    }

    //printf("%p\n%p\n\n", compiler->constants[0], compiler->constants[1]);
}

// =============================================================================
// manage defs
// =============================================================================

void init_definitions() {
    definitions = hash_map_new(60);
}

void free_definitions() {
    int i;
    SortedList * current = NULL;
    Definition * definition = NULL;

    for(i = 0; i < definitions->size; i++) {
        current = definitions->array[i];

        while(current != NULL) {
            definition = current->data;
            free(definition->name);
            free(definition->widths);
            current = current->next;
        }
    }

    hash_map_free(definitions);
}

char * op_code_to_str(int op) {
    char * z = malloc(20);

    sprintf(z, "%i", op);

    return z;
}

void comp_new_definition(int type) {
    char * name = malloc(20);
    int * widths = malloc(sizeof(int) * 1), wc;
    Definition *  new_def = malloc(sizeof(Definition));;

    switch(type) {
        case OP_CONSTANT:
            strcpy(name, "OP_CONSTANT");
            widths[0] = 2;
            wc = 1;
            break;
        case OP_ADD:
            strcpy(name, "OP_ADD");
            wc = 0;
            break;
        default: break;
    }

    new_def->name = name;
    new_def->widths = widths;
    new_def->wc = wc;

    hash_map_insert(definitions, op_code_to_str(type), new_def);
}

Definition * comp_lookup_op(byte op) {
    char * z = op_code_to_str(op);
    SortedList * find = hash_map_find(definitions, z);

    if(find == NULL) {
        return NULL;
    }

    free(z);

    return find->data;
}

// =============================================================================
//
// =============================================================================

Instruction * make(byte op, int operands[]) {
    Definition * definition = comp_lookup_op(op);
    int oc = sizeof(* operands) / sizeof(operands[0]);
    byte * instructions = malloc(sizeof(byte));
    int c = 1, offset = 1;
    Instruction * instruction = malloc(sizeof(Instruction));

    instructions[0] = op;

    if(definition == NULL) {
        return NULL;
    }

    for(int i = 0; i < oc; i++) {
        int width = definition->widths[i];

        switch(width) {
            case 1:
                instructions = realloc(instructions, sizeof(byte) * (c + 1));
                instructions[c++] = operands[i];
                break;
            case 2:
                instructions = realloc(instructions, sizeof(byte) * (c + 2));
                instructions[c++] = operands[i] >> 8;
                instructions[c++] = operands[i] >> 0;
                break;
            case 4:
                instructions = realloc(instructions, sizeof(byte) * (c + 4));
                instructions[c++] = operands[i] >> 24;
                instructions[c++] = operands[i] >> 16;
                instructions[c++] = operands[i] >> 8;
                instructions[c++] = operands[i] >> 0;
                break;
            case 8:
                instructions = realloc(instructions, sizeof(byte) * (c + 8));
                instructions[c++] = (u_int64_t) operands[i] >> 56;
                instructions[c++] = (u_int64_t) operands[i] >> 48;
                instructions[c++] = (u_int64_t) operands[i] >> 40;
                instructions[c++] = (u_int64_t) operands[i] >> 32;
                instructions[c++] = operands[i] >> 24;
                instructions[c++] = operands[i] >> 16;
                instructions[c++] = operands[i] >> 8;
                instructions[c++] = operands[i] >> 0;
                break;
        }

        offset += width;
    }

    instruction->instruction = instructions;
    instruction->instruction_length = offset;

    return instruction;
}

// =============================================================================
// test
// =============================================================================

Program * comp_test_parse(char * input) {
    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);

    return parse_program(parser);
}

// =============================================================================
// vmtest
// =============================================================================

const int stack_size = 2048;

typedef struct {
    Instruction * instructions;
    Object ** constants;
    int constant_count;
    Object ** stack;
    int stack_count;
    int sp;
} VM;

VM * new_vm(Bytecode * bytecode) {
    VM * vm = malloc(sizeof(VM));

    vm->instructions = bytecode->instructions;
    vm->constants = bytecode->constants;
    vm->constant_count = bytecode->constant_count;
    vm->sp = 0;
    vm->stack_count = 0;
    vm->stack = malloc(sizeof(Object *) * stack_size);

    return vm;
}

Object * vm_pop(VM * vm) {
    Object * o = vm->stack[vm->sp - 1];

    vm->sp--;

    return o;
}

Object * stack_top(VM * vm) {
    if(vm->sp == 0) {
        return NULL;
    }

    //printf("%p\n%p\n", vm->stack[0], vm->stack[1]);

    return vm->stack[vm->sp - 1];
}

void * vm_push(VM * vm, Object * obj) {
    if(vm->sp >= stack_size) {
        printf("stack overflow\n");
        return NULL;
    }

    vm->stack = realloc(vm->stack, sizeof(Object *) * (vm->stack_count + 1));
    vm->stack[vm->stack_count++] = obj;
    vm->sp++;

    return NULL;
}

void * run_vm(VM * vm) {
    int i;
    Instruction * inst = vm->instructions;
    byte const_index = 0;
    Object * left, * right;

    for(i = 0; i < inst->instruction_length; i++) {
        byte * ins = inst->instruction;
        byte op = ins[i];

        switch(op) {
            case OP_CONSTANT:
                const_index = const_index | (ins[i + 1] << 8);
                const_index = const_index | (ins[i + 2]);
                i += 2;
                vm_push(vm, vm->constants[const_index]);
                break;
            case OP_ADD:
                left = vm_pop(vm);
                right = vm_pop(vm);
                IntegerObject * left_int = left->value;
                IntegerObject * right_int = right->value;
                int result = left_int->value + right_int->value;
                printf("%i\n", result);
                vm_push(vm, new_int_from_val(result));
                break;
        }
    }
}

String * instruction_to_string(byte * instructions, int ic);
void run_vm_test(VMTest tests[], int tc) {
    int i;

    for(i = 0; i < tc; i++) {
        Program * program = comp_test_parse(tests[i].input);
        Compiler * compiler = compiler_new();
        compile(compiler, program);
        Bytecode * bc = compiler_get_bytecode(compiler);
        VM * vm = new_vm(bc);
        run_vm(vm);
        Object * obj = stack_top(vm);
        IntegerObject * iobj = obj->value;
        //printf("%i\n", iobj->value);
    }
}

void test_vm() {
   VMTest t[1] = {{"1 + 2", 2}};

   run_vm_test(t, 1);
}

// =============================================================================
// test_integer_arithmetic
// =============================================================================

byte * concat_instructions(Instruction ** ei, int ic, int * conc_sz) {
    int i, j, b, sz = 0;
    byte * concat = NULL;

    for(i = 0; i < ic; i++) {
        sz += ei[i]->instruction_length;
    }

    concat = malloc(sizeof(byte) * sz);

    for(i = 0, b = 0; i < ic; i++) {
        for(j = 0; j < ei[i]->instruction_length; j++, b++) {
            concat[b] = ei[i]->instruction[j];
            //printf("%i\n", ei[i]->instruction[j]);
            (*conc_sz) ++;
        }
    }

    return concat;
}

bool comp_test_integer_object(Object * obj, int test) {
    IntegerObject * v = obj->value;

    return v->value == test;
}

void test_constants(int * ec, int cc, Object ** actual, int ac) {
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

    //printf("%i %i\n", conc_sz, ins->instruction_length);

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
}

void run_compiler_tests(CompilerTest ** tests, int tc, int z[]) {
    int i;
    Program * program = NULL;
    Compiler * compiler = NULL;
    Bytecode * bytecode = NULL;

    for(i = 0; i < tc; i++) {
        program = comp_test_parse(tests[i]->input);
        compiler = compiler_new();

        if(compile(compiler, program) != NULL) {
            //break;
        }

        bytecode = compiler_get_bytecode(compiler);

        test_instructions(tests[i]->expected_instructions, tests[i]->ic,
            bytecode->instructions);

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
    test->expected_instructions = malloc(sizeof(Instruction *) * 3);

    int a[1] = {0}, b[1] = {1}, c[0] = {};
    test->expected_instructions[0] = make(OP_CONSTANT, a);
    test->expected_instructions[1] = make(OP_CONSTANT, b);
    test->expected_instructions[2] = make(OP_ADD, c);

    test->ic = 3;
    test->cc = 2;

    tests[0] = test;

    int v[2] = {1,2};

    run_compiler_tests(tests, 1, v);
}

// =============================================================================
// test_instruction_string
// =============================================================================

Operands * read_operands(Definition * def, byte * ins, int st);

char * format_instruction(Definition * def, int * operands, int oc) {
    if(oc != def->wc) {
        return "Invalid operand length";
    }

    switch(oc) {
        case 0: return def->name;
        case 1: return def->name;
    }

    return "Unhandled operand length for definition";
}

String * instruction_to_string(byte * instructions, int ic) {
    int i = 0, offset;
    char * fmt = malloc(sizeof(char) * 40), * def_name;
    String * msg = string_new();
    Definition * def = NULL;
    Operands * op = NULL;

    while(i < ic) {
        def = comp_lookup_op(instructions[i]);

        if(def == NULL) {
            continue;
        }

        op = read_operands(def, instructions, i + 1);
        def_name = format_instruction(def, op->operands, op->offset - 1);

        if(op->offset - 1 > 0) {
            sprintf(fmt, "%i %s %i\n", i, def_name, op->operands[0]);
            i += 1 + op->offset;
        } else {
            sprintf(fmt, "%i %s\n", i, def_name);
            i+=1;
        }

        string_cat(msg, fmt, 1);

    }

    return msg;
}

void test_instruction_string() {
    Instruction ** instructions = malloc(sizeof(Instruction *) * 4);
    int a[1] = {1}, b[1] = {2}, c[1] = {65535}, d[0] = {};
    char * expected = "0 OP_CONSTANT 1\n3 OP_CONSTANT 2\n6 OP_CONSTANT 65535\n9 OP_ADD\n";
    String * ins_str = NULL;
    byte * concat = NULL;
    int conc_sz = 0;

    instructions[0] = make(OP_CONSTANT, a);
    instructions[1] = make(OP_CONSTANT, b);
    instructions[2] = make(OP_CONSTANT, c);
    instructions[3] = make(OP_ADD, d);

    concat = concat_instructions(instructions, 4, &conc_sz);
    ins_str = instruction_to_string(concat, conc_sz);

    if(strcmp(ins_str->string, expected) != 0) {
        printf("Instructions wrongly formatted. Got \n%s\n Expected \n%s\n",
            ins_str->string, expected);
    }
}

// =============================================================================
// test_read_operands
// =============================================================================

void comp_operand_ins(int ** instructions, int * c, int v) {
    *instructions = realloc(*instructions, sizeof(int) * (* c + 1));
    *instructions[(* c)++] = v;
}

Operands * read_operands(Definition * def, byte * ins, int st) {
    int i, width;
    u_int64_t operand;
    Operands * op = malloc(sizeof(Operands));

    op->operands = malloc(sizeof(int));
    op->length = 0;
    op->offset = 0;

    if(def->wc == 0) {
        op->offset = 1;
        return op;
    }

    for(i = 0; i < def->wc; i++) {
        width = def->widths[i];

        switch(width) {
            case 1:
                operand = ins[st];
                break;
            case 2:
                operand = 0;
                operand = operand | (ins[st] << 8);
                operand = operand | (ins[st + 1]);
                break;
            case 4:
                operand = 0;
                operand = operand | (ins[st + 0] << 24);
                operand = operand | (ins[st + 1] << 16);
                operand = operand | (ins[st + 2] << 8);
                operand = operand | (ins[st + 3]);
                break;
            case 8:
                operand = 0;
                operand = operand | ((u_int64_t) ins[st + 0] << 56);
                operand = operand | ((u_int64_t) ins[st + 1] << 48);
                operand = operand | ((u_int64_t) ins[st + 2] << 40);
                operand = operand | ((u_int64_t) ins[st + 3] << 32);
                operand = operand | ((u_int64_t) ins[st + 4] << 24);
                operand = operand | ((u_int64_t) ins[st + 5] << 16);
                operand = operand | ((u_int64_t) ins[st + 6] << 8);
                operand = operand | ((u_int64_t) ins[st + 7]);
                break;
        }

        comp_operand_ins(&(op->operands), &op->length, operand);
        op->offset += width;
        st += width;
    }

    return op;
}

void free_instruction(Instruction * instruction) {
    free(instruction->instruction);
    free(instruction);
}

void free_operands(Operands * operands) {
    free(operands->operands);
    free(operands);
}

void test_read_operands() {
    int a[1] = {65535};
    int c = 1, i, j;
    struct {
        byte opcode;
        int * operands;
        int bytes_read;
    } t[1] = {0, a, 2};
    Instruction * instruction = NULL;
    Definition * definition = NULL;
    Operands * operands = NULL;

    for(i = 0; i < c; i++) {
        instruction = make(t[i].opcode, t[i].operands);
        definition = comp_lookup_op(t[i].opcode);
        operands = read_operands(definition, instruction->instruction, 1);

        if(operands->offset != t[i].bytes_read) {
            printf("Expected offset %i got %i\n", t[i].bytes_read,
                operands->offset);
        } else {
            printf("[%i] Passed: bytes read: %i, expected: %i\n",
                i, operands->offset, t[i].bytes_read);
        }

        for(j = 0; j < operands->length; j++) {
            if(t[i].operands[j] != operands->operands[j]) {
                printf("[%i] Expected operand %i got %i\n", i,
                    t[i].operands[j], operands->operands[j]);
            } else {
                printf("[%i] Index %i got: %i, expected: %i\n",
                    i, j, t[i].operands[j], operands->operands[j]);
            }
        }

        printf("[%i] Passed: operands match length: %i\n", i, operands->length);

        free_instruction(instruction);
        free_operands(operands);
    }
}

// =============================================================================

// =============================================================================

void comp_test() {
    init_definitions();
    comp_new_definition(OP_CONSTANT);
    comp_new_definition(OP_ADD);
    //SortedList * m = hash_map_find(definitions, "0");
    //test_integer_arithmetic();
    //test_instruction_string();
    test_read_operands();
    //test_vm();
    free_definitions();
}
