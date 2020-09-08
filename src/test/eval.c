/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * test/eval.c
 *
 */

Object * get_eval_object(char * input);
void test_eval_all(char * option);
char * test_prefix_expression_object();
char * test_error_object();
char * test_return_object();
char * test_integer_object();
char * test_boolean_object();

void test_eval_all(char * option) {
    int i, c;

    if(strcmp(option, "integer") == 0) {
        printf("%s", test_integer_object());
    } else if(strcmp(option, "boolean") == 0) {
        printf("%s", test_boolean_object());
    } else if(strcmp(option, "return") == 0) {
        printf("%s", test_return_object());
    } else if(strcmp(option, "error") == 0) {
        printf("%s", test_error_object());
    } else {
        c = 4;

        char * s[4] = {
            test_error_object(),
            test_return_object(),
            test_integer_object(),
            test_boolean_object()};

        printf("\n");

        for(i = 0; i < c; i++) {
            printf("%s", s[i]);
        }
    }
}

Object * get_eval_object(char * input) {
    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    return eval_statement(program->statements[0]);
}

char * test_error_object() {
    int i, tc = 3, fail = 0;
    Statement stmt;
    Object * obj;
    ErrorObject * eobj;
    struct {
        char * input;
    } t[3] = {
        {"true + true;"},
        {"true + 5;"},
        {"-false;"}};

    printf("Testing ERROR object\n");

    for(i = 0; i < tc; i++) {
        obj = get_eval_object(t[i].input);
        eobj = obj->value;

        printf("[%i] %s: %s", i, t[i].input, inspect_error_object(eobj));

        if(!test_string_cmp("[Error: %i] Expected object type %s got %s\n",
            ERROR, obj->type, i, &fail)) {
        }
    }

    return print_test_result("ERROR", tc - fail, tc);
}


char * test_return_object() {
    int i, tc = 4, fail = 0;
    Statement stmt;
    Object * obj, * ret;
    struct {
        char * input, * type;
    } t[4] = {
        {"return 33;", INT},
        {"return asd;", ""},
        {"if(1) { if(1 > 2) {} else { return 5;} } else { return var; }", INT},
        {"return !true;", FALSE}};

    printf("Testing RETURN object\n");

    for(i = 0; i < tc; i++) {
        obj = get_eval_object(t[i].input);
        ret = (Object *) obj->value;

        printf("[%i] %s: %s\n", i, t[i].input, t[i].type);

        test_string_cmp("[Error: %i] Expected object type %s got %s\n",
            t[i].type, ret->type, i, &fail);
    }

    return print_test_result("RETURN", tc - fail, tc);
}

char * test_boolean_object() {
    int i, tc = 5, fail = 0;
    Statement stmt;
    Object * obj;
    BooleanObject * bobj;
    struct {
        char * input, * type;
        bool value;
    } t[5] = {
        {"!(true == true)", FALSE, false},
        {"false", FALSE, false},
        {"!true", FALSE, false},
        {"!!true", TRUE, true},
        {"!!!false", TRUE, true}};

    printf("Testing BOOLEAN object\n");

    for(i = 0; i < tc; i++) {
        obj = get_eval_object(t[i].input);
        bobj = obj->value;

        if(!test_string_cmp("[Error: %i] Expected object type %s got %s\n",
            t[i].type, obj->type, i, &fail)) {
            continue;
        }

        printf("[%i] %s: %i\n", i, t[i].input, inspect_boolean_object(bobj));

        test_int_cmp("[Error: %i] Expected boolean object value %i got %i\n",
            t[i].value, bobj->value, i, &fail);
    }

    return print_test_result("BOOLEAN", tc - fail, tc);
}

char * test_integer_object() {
    int i, tc = 6, fail = 0;
    Statement stmt;
    Object * obj;
    IntegerObject * iobj;
    BooleanObject * bobj;
    struct {
        char * input;
        int value;
    } t[6] = {
        {"3 * (3 * 3) + 10", 37},
        {"!1", false},
        {"!!0", true},
        {"-10", -10},
        {"-5", -5},
        {"5", 5}};

    printf("Testing INTEGER object\n");

    for(i = 0; i < tc; i++) {
        obj = get_eval_object(t[i].input);

        if(obj == false_bool || obj == true_bool) {
            bobj = obj->value;
            printf("[%i] %i\n", i, inspect_boolean_object(bobj));
            test_int_cmp("[Error: %i] Expected bool object value %i got %i\n",
                t[i].value, bobj->value, i, &fail);
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
