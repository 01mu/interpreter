/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/builtin.c
 *
 */

Object * built_in_int_obj(int i) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = i;
    obj->type = INT;
    obj->value = iobj;

    return obj;
}

Object * built_in_len(Object * obj, Object ** args, int argc) {
    int i, str_len = 0;
    char * arg_type = args[0]->type, * m = NULL;

    if(strcmp(arg_type, STRING) == 0) {
        str_len = ((String *) ((StringObject *) args[0]->value)->value)->len;
    }

    for(i = 0; i < argc; i++) {
        free_eval_expression(args[i]->type, args[i], NULL, true);
    }

    free_eval_expression(obj->type, obj, NULL, true);

    if(argc != 1) {
        m = malloc(sizeof(char) * 20);
        sprintf(m, "Too many arguments");
        return new_error(m);
    } else if(strcmp(arg_type, STRING) != 0) {
        m = malloc(sizeof(char) * 30);
        sprintf(m, "Argument not a string");
        return new_error(m);
    } else {
        return built_in_int_obj(str_len);
    }
}
