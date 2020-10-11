/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/infix/array.c
 *
 */

bool eval_array_infix_comp(Array * arl, Array * arr) {
    int i;
    Object * lo = NULL, * ro = NULL;
    char * lot = NULL, * rot = NULL;
    StringObject * los = NULL, * ros = NULL;

    if(arl->size != arr->size) {
        return false;
    }

    for(i = 0; i < arl->size; i++) {
        lo = arl->array[i];
        ro = arr->array[i];
        lot = lo->type;
        rot = ro->type;

        if(strcmp(lot, INT) == 0 && strcmp(rot, INT) == 0) {
            if(((IntegerObject *) lo->value)->value !=
                ((IntegerObject *) ro->value)->value) {

                return false;
            }
        } else if(strcmp(lot, STRING) == 0 && strcmp(rot, STRING) == 0) {
            los = lo->value;
            ros = ro->value;

            if(strcmp(((String *) los->value)->string,
                ((String *) ros->value)->string) != 0) {

                return false;
            }
        } else if(strcmp(lot, BOOLEAN) == 0 && strcmp(rot, BOOLEAN) == 0) {
            if(lo->value != ro->value) {
                return false;
            }
        } else if(strcmp(lot, ARRAY) == 0 && strcmp(rot, ARRAY) == 0) {
            return eval_array_infix_comp(((ArrayObject *) lo->value)->elements,
                ((ArrayObject *) ro->value)->elements);
        } else {
            return false;
        }
    }

    return true;
}

Object * eval_array_infix_exp(char * op, Object * l, Object * r, Env * env) {
    int i;
    ArrayObject * all = l->value, * alr = r->value;
    Array * arl = all->elements, * arr = alr->elements, * new_arr = NULL;
    Object * ret = NULL;

    if(strcmp(op, "==") == 0) {
        ret = eval_bool(eval_array_infix_comp(arl, arr), env);
    } else if(strcmp(op, "!=") == 0) {
        ret = eval_bool(!eval_array_infix_comp(arl, arr), env);
    } else if(strcmp(op, "+") == 0) {
        ret = eval_new_array();
        new_arr = ((ArrayObject *) ret->value)->elements;

        for(i = 0; i < arl->size; i++) {
            array_insert(new_arr, copy_object(arl->array[i]));
        }

        for(i = 0; i < arr->size; i++) {
            array_insert(new_arr, copy_object(arr->array[i]));
        }
    }

    return ret;
}
