/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/builtin.c
 *
 */

Object * bi_int_obj(int i) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = i;
    obj->type = INT;
    obj->value = iobj;

    return obj;
}

void bi_free_args(Object * obj, Object ** args, int argc) {
    int i;

    for(i = 0; i < argc; i++) {
        free_eval_expression(args[i]->type, args[i], NULL, true);
    }

    free_eval_expression(obj->type, obj, NULL, true);
}

Object * bi_str(Object * obj, Object ** args, int argc) {
    char * arg_0_type = args[0]->type, * m = NULL;

    free_eval_expression(obj->type, obj, NULL, true);

    if(argc != 1) {
        m = malloc(sizeof(char) * 40);
        bi_free_args(obj, args, argc);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 1, argc);
        return new_error(m);
    } else if(strcmp(arg_0_type, STRING) == 0) {
        m = malloc(sizeof(char) * 30);
        bi_free_args(obj, args, argc);
        sprintf(m, "Argument already a string");
        return new_error(m);
    } else {
        Object * new = NULL;
        StringObject * nv = NULL;
        String * new_string = NULL;

        new = malloc(sizeof(Object));
        nv = malloc(sizeof(StringObject));
        new_string = string_new();

        char * l = malloc(sizeof(char) * 30);
        l[0] = '\0';
        sprintf(l, "%i", ((IntegerObject *) args[0]->value)->value);

        string_cat(new_string, l, 1);

        new->type = STRING;
        new->value = nv;
        nv->value = new_string;

        free_eval_expression(INT, args[0], NULL, true);

        return new;
    }
}

Object * bi_find(Object * obj, Object ** args, int argc) {
    char * arg_0_type = args[0]->type, * arg_1_type = args[1]->type, * m = NULL;

    if(argc != 2) {
        m = malloc(sizeof(char) * 40);
        bi_free_args(obj, args, argc);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 2, argc);
        return new_error(m);
    } else if(strcmp(arg_0_type, STRING) != 0 ||
        strcmp(arg_1_type, STRING) != 0) {

        m = malloc(sizeof(char) * 30);
        bi_free_args(obj, args, argc);
        sprintf(m, "Argument not a string");
        return new_error(m);
    } else {
        StringObject * hay = args[0]->value, * needle = args[1]->value;
        String * hh = hay->value, * nn = needle->value;
        char * p = strstr(hh->string, nn->string);
        int f = -1;

        if(p) {
            f = p - hh->string;
        }

        bi_free_args(obj, args, argc);
        return bi_int_obj(f);
    }
}

Object * bi_len(Object * obj, Object ** args, int argc) {
    char * arg_type = args[0]->type, * m = NULL;

    if(argc != 1) {
        m = malloc(sizeof(char) * 40);
        bi_free_args(obj, args, argc);
        sprintf(m, "Invalid args (expected: %i, got: %i)", 1, argc);
        return new_error(m);
    } else if(strcmp(arg_type, STRING) != 0) {
        m = malloc(sizeof(char) * 30);
        bi_free_args(obj, args, argc);
        sprintf(m, "Argument not a string");
        return new_error(m);
    } else {
        int str_len;

        str_len = ((String *) ((StringObject *) args[0]->value)->value)->len;
        bi_free_args(obj, args, argc);
        return bi_int_obj(str_len);
    }
}
