/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * test/eval.c
 *
 */

Object * test_eval(char * input) {
    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);

    return eval_statement(program->statements[0]);
}

char * test_integer_object() {
    int i, tc = 1, fail = 0;
    Statement stmt;
    Object * obj;
    IntegerObject * iobj;
    struct {
        char * input;
        int value;
    } t[1] = {
        {"5", 5}};

    printf("Testing INTEGER object\n");

    for(i = 0; i < tc; i++) {
        obj = test_eval(t[i].input);
        iobj = obj->value;

        if(!test_string_cmp("[Error: %i] Expected object type %s got %s\n",
            INT, obj->type, i, &fail)) {
            continue;
        }

        printf("%i\n", inspect_integer_object(iobj));

        test_int_cmp("[Error: %i] Expected integer object value %i got %i\n",
            t[i].value, iobj->value, i, &fail);
    }

    return print_test_result("INTEGER", tc - fail, tc);
}
