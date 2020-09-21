/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/repl.c
 *
 */

void repl_test();
void repl(bool is_test, bool * good, char ** input, int * j);
void free_function_literal_store();
void free_env_store();

void repl_test() {
    char * t[][2] = {
        {"let c = fn(x) { if(x < 5) { c(x + 1); } else { 5; } }; c(0);", "5 "},
        {"let a = 1; if(true) { let a = 55; } else { }; a;", "1 55 "},
        {"let e = 1; let a = fn(z) { if(true) { let f = 3; } }; a(e); e;",
            "1 3 1 "},
        {"let a = fn() { if(true) { if(false) { } else { return 5; }; } \
            else { }; }; let a = a(); a; a; a;", "5 5 5 "},
        {"!!!false == true", "1 "},
        {"a/2;", "Identifier not found: a "},
        {"2/a;", "Identifier not found: a "},
        {"return a; 1;", "Identifier not found: a "},
        {"let z = 1; let a = fn(z) { if(true) { return 33; } }; \
            let z = a(z); z;", "1 33 "},
        {"true; false; !true; !false; true == false;", "1 0 0 1 0 "},
        {"let a = fn() { let b = 3; return b; }; a();", "3 3 "},
        {"let a = 2; let b = a; let a = b + 2; a; 3;", "2 4 3 "},
    };

    int e = sizeof(t) / sizeof(t[0]);
    //e = 1;
    int b = e, i;
    bool res = false;

    /*char * t[] = {
        "return a;",
        "return 1;",
        "let a = fn() { if(false) { 1; 2; return 3; 44; let z = 1; } \
            else { } }; let z = a(); let z = 22; z;",
        "let c = fn(x) { if(x > 5) { return true; } \
            else { x; c(x + 1); } }; c(0);",
        "let n = 2; let a = fn(x) { let z = 2; let z=2; let n = z * 2; \
            return n + x; }; a(3) + 2;",
        "let a = fn(x) { if(x) { return 44; } else { return 42; } }; \
            let f= a(33); f;",
        "let a = fn(x) { let z = fn(x, y) { if(x > y) { return true; } \
            else { return false; } }; z(1, 0); }; a(1);",
        "let a = fn(x) { x; }; a(z);",
        "!5 == 5",
        "2/a;",
        "z/2;",
        "1>2; true == true; !false == 0;",
        "let a = 2; let b = a; a; b; let a = 55; a; b;",
        "let a = 2; let b = 3; let a = b + 2;",
        "let a = fn() { if(true) { if(false) { } else { return 5; }; } \
            else { }; }; let a = a(); a; a; a;",
        "let z = fn() { if(!true) { return 1; } else { false; } }; \
            let a = z(); a; a;",
        "let c = fn(x) { if(2 < 1) { true; } else { x; 3; } }; \
            let z = c(3);",
        "let a = fn() { let z = false; z; return 3; }; a();",
        "let a = fn(x) { if(!true) { 1; } else { 2; } }; a(2);",
        "let a = fn(x) { return 1; }; let a = 3; a; \
            let a = fn(x) { return 1; }; a(3);",
        "let mult = fn(x) { 2; 2; x; return 3; }; mult(33);",
        "let a = fn(x) { return 1; }; a(1);",
        "let mult = fn(x) { let z = 2; 2; 2; x; 3; }(33);",
        "let mult = fn(x) { 2; 2; x; return 3; }; let a = mult(33);",
        "let mult = fn(x) { 2; 2; x; 3; }; mult(33);",
        "let mult = fn(x) { 17; }; let j = mult(33);",
        "let mult = fn(x, a) { 1; 33; x; }; let z = 2; mult(z, 10);",
        "let mult = fn(x, a) { x * a; }; let b = mult(3, 10);",
        "let a = true; let a = false; let b = a; a; !b;",
        "let var = 1 ---3; let z = -1; z * -2;",
        "let a = !3; if(!!!!false == true) { a } else { 55 }",
        "!false; !!true; false; true; !5; !!5; !!!500; -5;",
        "let a = 3; if(a + 1) { 5; } else {  };",
        "let a = 3; let b = 5; let z = 3; 1 + 2;",
        "let a = 2; let b = a; let a = b + 2; 3;",
    };*/

    for(i = 0; i < b; i++) {
        res = false;
        repl(1, &res, t[i], &i);

        if(!res) {
            --e;
        }
    }

    if(e == b) {
        printf(ANSI_COLOR_GREEN "\n[PASSED] " ANSI_COLOR_RESET
            "REPL Test | %i out of %i passed\n", e, b);
    } else {
        printf(ANSI_COLOR_RED "\n[FAILED] " ANSI_COLOR_RESET
            "REPL Test | %i out of %i passed\n", e, b);
    }

}

void repl(bool is_test, bool * good, char ** input, int * j) {
    int i;
    char str[120];
    Lexer * lexer = NULL;
    Parser * parser = NULL;
    Program * program = NULL;
    Env * env = new_env();

    fls = malloc(sizeof(FunctionLiteralStore));
    fls->count = 0;
    fls->store = malloc(sizeof(FunctionLiteral *));

    env_store = malloc(sizeof(EnvStore));
    env_store->count = 0;
    env_store->store = malloc(sizeof(Env *));

    if(is_test) {
        repl_test_string = string_new();
        is_repl_test_string = true;
    }

    env_store_add(env);

    if(!is_test) {
        printf("Type '\\h' for help and '\\q' to quit.\n");
    }

    for( ; ; ) {
        if(!is_test) {
            printf(">>> ");
            fgets(str, 120, stdin);
        }

        if(!is_test) {
            lexer = new_lexer(str);

            if(strcmp(str, "\\q\n") == 0) {
                printf("Bye!\n");
                break;
            } else if(strcmp(str, "\\e\n") == 0) {
                free(lexer);
                env_display(env);
                continue;
            } else if(strcmp(str, "\\h\n") == 0) {
                printf("Commands:\nDisplay environment: \\e\n");
                free(lexer);
                continue;
            }
        } else {
            lexer = new_lexer(input[0]);
        }

        parser = new_parser(lexer);
        program = parse_program(parser);

        if(check_parser_errors(parser)) {
            free_env_store();
            free_program(lexer, parser, program);
            continue;
        }

        if(program->sc > 0) {
            eval_statements(program->statements, program->sc, env);
        }

        free_env_store();
        free_program(lexer, parser, program);

        if(is_test) {
            break;
        }
    }

    env_free((Env *) env_store->store[0]);
    free_function_literal_store();
    free(env_store->store);
    free(env_store);

    if(!is_test) {
        free(lexer);
    } else {
        if(strcmp(repl_test_string->string, input[1]) == 0) {
            * good = true;
        } else {
            printf(ANSI_COLOR_RED "[FAILED] " ANSI_COLOR_RESET
                "Index: %i | Expected: \"%s\" | Got: \"%s\"\n",
                * j, input[1], repl_test_string->string);
        }

        string_free(repl_test_string);
    }
}

void free_function_literal_store() {
    int i;

    for(i = 0; i < fls->count; i++) {
        free_function_literal((FunctionLiteral *) fls->store[i]);
    }

    free(fls->store);
    free(fls);
}

void free_env_store() {
    int i, nc = 0;

    for(i = 1; i < env_store->count; i++, nc++) {
        env_free((Env *) env_store->store[i]);
    }

    env_store->count -= nc;
}
