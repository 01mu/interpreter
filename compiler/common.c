/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * compiler/common.c
 *
 */

void free_instruction(Instruction * instruction) {
    free(instruction->instruction);
    free(instruction);
}

void free_operands(Operands * operands) {
    free(operands->operands);
    free(operands);
}

Compiler * compiler_new() {
    Compiler * compiler = malloc(sizeof(Compiler));

    compiler->instructions = malloc(sizeof(Instruction));
    compiler->instructions->instruction = malloc(sizeof(byte));
    compiler->instructions->instruction_length = 0;
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

Instruction * make(byte op, int operands[]) {
    Definition * definition = comp_lookup_op(op);
    int oc = sizeof(* operands) / sizeof(operands[0]);
    byte * instructions = malloc(sizeof(byte));
    int c = 1, offset = 1;
    Instruction * instruction = malloc(sizeof(Instruction));

    instructions[0] = op;
    instruction->instruction = instructions;
    instruction->instruction_length = 1;

    if(definition == NULL) {
        return NULL;
    }

    if(definition->wc == 0) {
        return instruction;
    }

    for(int i = 0; i < oc; i++) {
        int width = definition->widths[i];

        switch(width) {
            case 0: break;
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

Program * comp_test_parse(char * input) {
    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);

    return parse_program(parser);
}

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
            (*conc_sz) ++;
        }
    }

    return concat;
}

Operands * read_operands(Definition * def, byte * ins, int st);
