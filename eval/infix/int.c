/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/infix/int.c
 *
 */

Object * eval_integer_infix_exp(char * op, Object * l, Object * r, Env * env) {
    Object * obj = NULL;
    IntegerObject * int_obj = NULL;
    char * m = NULL;

    int left = ((IntegerObject *) l->value)->value;
    int right = ((IntegerObject *) r->value)->value;

    int arith = 0;

    if(strcmp(op, "+") == 0) {
        arith = left + right;
    } else if(strcmp(op, "-") == 0) {
        arith = left - right;
    } else if(strcmp(op, "*") == 0) {
        arith = left * right;
    } else if(strcmp(op, "/") == 0) {
        arith = left / right;
    } else if(strcmp(op, "<=") == 0) {
        obj = eval_bool(left <= right, env);
    } else if(strcmp(op, ">=") == 0) {
        obj = eval_bool(left >= right, env);
    } else if(strcmp(op, "<") == 0) {
        obj = eval_bool(left < right, env);
    } else if(strcmp(op, ">") == 0) {
        obj = eval_bool(left > right, env);
    } else if(strcmp(op, "==") == 0) {
        obj = eval_bool(left == right, env);
    } else if(strcmp(op, "!=") == 0) {
        obj = eval_bool(left != right, env);
    } else if(strcmp(op, "&&") == 0) {
        obj = eval_bool(is_truthy(l) && is_truthy(r), env);
    } else if(strcmp(op, "||") == 0) {
        obj = eval_bool(is_truthy(l) || is_truthy(r), env);
    } else {
        m = malloc(strlen(op) + strlen(r->type) + strlen(l->type) + 25);
        sprintf(m, "Unknown operator: %s %s %s", l->type, op, r->type);
        return new_error(m);
    }

    if(strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 ||
        strcmp(op, "/") == 0) {

        obj = malloc(sizeof(Object));
        int_obj = malloc(sizeof(IntegerObject));
        int_obj->value = arith;
        obj->type = INT;
        obj->value = int_obj;
        obj->ref = 0;
    }

    return obj;
}
