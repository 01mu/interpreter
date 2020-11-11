/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * compiler/test/readoperands.c
 *
 */

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

void test_read_operands() {
    int a[1] = {65535};
    int c = 1, i, j;
    struct {
        byte opcode;
        int * operands;
        int bytes_read;
    } t[1] = {OP_CONSTANT, a, 2};
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
