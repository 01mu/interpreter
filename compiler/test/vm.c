/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * compiler/test/vm.c
 *
 */

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
    vm->stack = malloc(sizeof(Object *));

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

void free_vm(VM * vm) {
    int i;

    //free_instruction(vm->instructions);

    for(i = 0; i < vm->stack_count; i++) {
        free(vm->stack[i]->value);
        free(vm->stack[i]);
    }

    for(i = 0; i < vm->constant_count; i++) {
        //free(vm->constants[i]->value);
        //free(vm->constants[i]);
    }

    free(vm->stack);
    //free(vm->constants);
    free(vm);
}

void run_vm_test(VMTest tests[], int tc) {
    int i;

    for(i = 0; i < tc; i++) {
        Lexer * lexer = new_lexer(tests[i].input);
        Parser * parser = new_parser(lexer);
        Program * program = parse_program(parser);
        Compiler * compiler = compiler_new();

        compile(compiler, program);

        Bytecode * bytecode = compiler_get_bytecode(compiler);
        VM * vm = new_vm(bytecode);

        run_vm(vm);

        free_vm(vm);
        free_compiler(compiler);
        free_program(lexer, parser, program);
    }
}

void test_vm() {
   VMTest t[1] = {{"1 + 2", 2}};

   run_vm_test(t, 1);
}
