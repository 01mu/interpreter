/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/copy.c
 *
 */

Object * copy_object(Object * obj);

Object * copy_integer_object(Object * fr) {
    Object * obj = malloc(sizeof(Object));
    IntegerObject * iobj = malloc(sizeof(IntegerObject));

    iobj->value = ((IntegerObject *) fr->value)->value;
    obj->type = INT;
    obj->value = iobj;

    return obj;
}

Object * copy_string_object(Object * obj) {
    Object * new = malloc(sizeof(Object));
    StringObject * nv = malloc(sizeof(StringObject));
    String * old_string = ((StringObject *) obj->value)->value;
    String * new_string = string_new();

    string_cat(new_string, old_string->string, 0);

    new->type = STRING;
    new->value = nv;
    nv->value = new_string;

    return new;
}

Object * copy_array_object(Object * obj) {
    int i;
    Object * new = malloc(sizeof(Object));
    ArrayObject * newao = malloc(sizeof(ArrayObject));
    Array * newarr = array_new();

    new->type = ARRAY;
    new->value = newao;
    newao->elements = newarr;

    ArrayObject * ao = obj->value;
    Array * ar = ao->elements;

    for(i = 0; i < ar->size; i++) {
        array_insert(newarr, copy_object(ar->array[i]));
    }

    return new;
}

Object * copy_function_object(Object * obj) {
    Object * new = malloc(sizeof(Object));
    Function * old = obj->value;
    Function * newf = malloc(sizeof(Function));

    newf->pc = old->pc;
    newf->env = old->env;
    newf->body = old->body;
    newf->parameters = old->parameters;

    new->type = FUNCTION;
    new->value = newf;

    return new;
}

Object * copy_object(Object * obj) {
    Object * ret = NULL;

    if(strcmp(obj->type, STRING) == 0) {
        ret = copy_string_object(obj);
    } else if(strcmp(obj->type, INT) == 0) {
        ret = copy_integer_object(obj);
    } else if(strcmp(obj->type, ARRAY) == 0) {
        ret = copy_array_object(obj);
    } else if(strcmp(obj->type, FUNCTION) == 0) {
        ret = copy_function_object(obj);
    } else {
        return obj;
    }

    return ret;
}
