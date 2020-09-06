/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * test/eval.c
 *
 */

Object * test_eval(char * input);
void test_eval_all();
char * test_prefix_expression_object();
char * test_integer_object();
char * test_boolean_object();

Object * test_eval(char * input) {
    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);

    return eval_statement(program->statements[0]);
}

void test_eval_all() {
    int i, c = 2;

    char * s[2] = {
        test_integer_object(),
        test_boolean_object()};

    printf("\n");

    for(int i = 0; i < c; i++) {
        if(i > 0) {
            free(s[i-1]);
        }

        printf("%s", s[i]);
    }

    free(s[c - 1]);
}

char * test_boolean_object() {
    int i, tc = 4, fail = 0;
    Statement stmt;
    Object * obj;
    BooleanObject * bobj;
    struct {
        char * input, * type;
        bool value;
    } t[4] = {
        {"true", TRUE, true},
        {"false", FALSE, false},
        {"!true", FALSE, false},
        {"!!true", TRUE, true}};

    printf("Testing BOOLEAN object\n");

    for(i = 0; i < tc; i++) {
        obj = test_eval(t[i].input);
        bobj = obj->value;

        if(!test_string_cmp("[Error: %i] Expected object type %s got %s\n",
            t[i].type, obj->type, i, &fail)) {
            continue;
        }

        printf("[%i]% i\n", i, inspect_boolean_object(bobj));

        test_int_cmp("[Error: %i] Expected boolean object value %i got %i\n",
            t[i].value, bobj->value, i, &fail);
    }

    return print_test_result("BOOLEAN", tc - fail, tc);
}

char * test_integer_object() {
    int i, tc = 5, fail = 0;
    Statement stmt;
    Object * obj;
    IntegerObject * iobj;
    BooleanObject * bobj;
    struct {
        char * input;
        int value;
    } t[5] = {
        {"5 + 3", 8},
        {"!!!!-5", 1},
        {"-10", -10},
        {"-5", -5},
        {"5", 5}};

    printf("Testing INTEGER object\n");

    for(i = 0; i < tc; i++) {
        obj = test_eval(t[i].input);

        if(obj == false_bool || obj == true_bool) {
            bobj = obj->value;
            printf("[%i] %i\n", i, inspect_boolean_object(bobj));
            continue;
        }

        iobj = obj->value;

        if(!test_string_cmp("[Error: %i] Expected object type %s got %s\n",
            INT, obj->type, i, &fail)) {
            continue;
        }

        printf("[%i] %i\n", i, inspect_integer_object(iobj));

        test_int_cmp("[Error: %i] Expected integer object value %i got %i\n",
            t[i].value, iobj->value, i, &fail);
    }

    return print_test_result("INTEGER", tc - fail, tc);
}
