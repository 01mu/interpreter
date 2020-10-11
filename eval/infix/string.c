/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/infix/string.c
 *
 */

Object * eval_string_infix_exp(char * op, Object * l, Object * r, Env * env) {
    char * m = NULL;
    String * lss = ((StringObject *) l->value)->value;
    String * rss = ((StringObject *) r->value)->value;
    Object * ret = NULL;

    if(strcmp(op, "+") == 0) {
        Object * new = eval_new_string();
        StringObject * sobj = new->value;

        string_cat(sobj->value, lss->string, 0);
        string_cat(sobj->value, rss->string, 0);

        ret = new;
    } else if(strcmp(op, "==") == 0) {
        ret = eval_bool(strcmp(lss->string, rss->string) == 0, env);
    } else if(strcmp(op, "!=") == 0) {
        ret = eval_bool(strcmp(lss->string, rss->string) != 0, env);
    } else {
        m = malloc(strlen(op) + strlen(r->type) + strlen(l->type) + 25);
        sprintf(m, "Unknown operator: %s %s %s", l->type, op, r->type);
        ret = new_error(m);
    }

    return ret;
}
