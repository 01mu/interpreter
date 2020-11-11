/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * compiler/test/insstring.c
 *
 */

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

    char * fmt = malloc(sizeof(char) * 50),
        * def_name = malloc(sizeof(char) * 100);

    String * msg = string_new();
    Definition * def = NULL;
    Operands * op = NULL;

    while(i < ic) {
        def = comp_lookup_op(instructions[i]);

        if(def == NULL) {
            continue;
        }

        op = read_operands(def, instructions, i + 1);
        strcpy(def_name, format_instruction(def, op->operands, op->offset - 1));

        if(op->offset - 1 > 0) {
            sprintf(fmt, "%i %s %i\n", i, def_name, op->operands[0]);
            i += 1 + op->offset;
        } else {
            sprintf(fmt, "%i %s\n", i, def_name);
            i += 1;
        }

        string_cat(msg, fmt, 1);
        fmt = malloc(sizeof(char) * 50);
        free(def_name);
        def_name = malloc(sizeof(char) * 100);
        free_operands(op);
    }

    free(fmt);
    free(def_name);

    return msg;
}

void test_instruction_string() {
    int i, conc_sz = 0, count = 4;
    Instruction ** instructions = malloc(sizeof(Instruction *) * count);
    char * expected =
        "0 OP_CONSTANT 1\n3 OP_CONSTANT 2\n6 OP_CONSTANT 65535\n9 OP_ADD\n";
    String * ins_str = NULL;
    byte * concat = NULL;
    int a[1] = {1}, b[1] = {2}, c[1] = {65535}, d[0] = {};

    instructions[0] = make(OP_CONSTANT, a);
    instructions[1] = make(OP_CONSTANT, b);
    instructions[2] = make(OP_CONSTANT, c);
    instructions[3] = make(OP_ADD, d);

    concat = concat_instructions(instructions, count, &conc_sz);
    ins_str = instruction_to_string(concat, conc_sz);

    if(strcmp(ins_str->string, expected) != 0) {
        printf("Instructions wrongly formatted. Got \n%s\n Expected \n%s\n",
            ins_str->string, expected);
    } else {
        printf("Test passed\n");
    }

    printf("%s\n", ins_str->string);

    for(i = 0; i < count; i++) {
        free_instruction(instructions[i]);
    }

    free(instructions);
    free(concat);
    string_free(ins_str);
}
