/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/compiler.c
 *
 */

int op_count;

typedef struct {
    char * name;
    int * widths;
    int wc;
} Definition;

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
    sprintf(z, "%c", op);
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

    for(i = 0; i < 4; i++) {
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

char * make(byte op, int * operands, int oc) {
    SortedList * sl = hash_map_find(definitions, op_code_to_str(op));
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

    return instruction;
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

void comp_test() {
    init_definitions();
    create_new_definition();
}
