/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * eval/bool.c
 *
 */

Object * eval_bool(bool b, Env * env) {
    if(b) {
        return true_bool;
    }

    return false_bool;
}
