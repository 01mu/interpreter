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
        // =====================================================================
        {"let c = fn(x) { if(x < 9) { return c(x + 1); } else { return x; } }; \
            let e = c(0); e + 1;", "9 10 "},
        {"let a = fn() { let b = 3; return b; }; a();", "3 3 "},
        {"let a = fn() { return fn(x, y) { 1; return x + y; }; }; \
            let g = a()(1, 2); g;", "1 3 3 "},
        {"let a = 1; if(true) { let a = 55; } else { }; a;", "1 55 "},
        {"let e = 1; let a = fn(z) { if(true) { let f = 3; } }; a(e); e;",
            "1 3 1 "},
        {"let a = fn() { if(true) { if(false) { } else { return 5; }; } \
            else { }; }; let a = a(); a; a; a;", "5 5 5 "},
        {"!!!false == true", "1 "},
        {"a/2;", "Identifier not found: a "},
        {"2/a;", "Identifier not found: a "},
        {"return a; 1;", "Identifier not found: a "},
        // 10 ==================================================================
        {"let z = 1; let a = fn(z) { if(true) { return 33; } }; \
            let z = a(z); z;", "1 33 "},
        {"true; false; !true; !false; true == false;", "1 0 0 1 0 "},
        {"let a = 2; let b = a; let a = b + 2; a; 3;", "2 4 3 "},
        {"let v = fn(x) { return x; }(3); v;", "3 3 "},
        {"let a = fn() { let b = 1; }; a(); a();", "1 1 "},
        {"let a = fn(x) { return 1; }; let a = 3; a; let a = fn(x) \
            { return 1; }; a(3);", "3 1 "},
        {"let a = fn() { 11; return 5; ff; }; let d = a(); d;", "11 5 5 "},
        {"let z = 1; let a = fn() { let z = 3; return z; }; \
            let d = a(); z; d;", "1 3 3 3 "},
        {"let z = false; let a = fn(z) { let z = true; }; a(z);", "0 " },
        {"let c = fn(x) { let g = fn() { return 5; }; return g() + 1; }; \
            let e = c(0); e * e;", "6 36 "},
        // 20 ==================================================================
        {"let c = fn(x, r) { if(x < r) { return c(x + 1, r); } else \
            { return x; } }; let g = fn() { return 12; }; let e = c(0, g()); \
            e * 2;", "12 24 "},
        {"let c = fn(x, g) { if(x < g) { return c(x + 1, g); } else \
            { return x + 1; } }; let g = 20; let e = fn() { c(0, g); }; e(); \
            g;", "20 21 20 "},
        {"\"asd\"; let z = \"ddd\";", "asd ddd "},
        {"let c = fn(x) { if(x < 9) { return c(x + 1); } else \
            { return \"a\"; } }; let e = c(0); e;", "a a "},
        {"let a = 1; let a = \"z\"; a;", "1 z "},
        {"let a = \"z\"; let a = \"d\"; a;", "z d "},
        {"let z = fn(a) { return a; }; let a = z(\"d\");", "d "},
        {"let z = fn(a) { return a; }; z(\"d\");", "d "},
        {"let g = \"z\"; let z = fn(a) { return a; }; z(g);", "z z "},
        {"let z = fn() { let g = \"zzz\"; return g; }; let d = z(); \
            let b = z();", "zzz zzz zzz zzz "},
        // 30 ==================================================================
        {"let a = \"a\"; let b = a; b;", "a a " },
        {"let f = \"abc\" + \"def\"; f;", "abcdef abcdef "},
        {"\"abc\" + \"def\";", "abcdef "},
        {"1 + 2;", "3 "},
        {"\"asd\" - \"a\"", "Unknown operator: STRING-STRING "},
        {"let z = \"a\"; let d = z + \"b\" + \"c\" + \"a\" + \"b\" + \"c\"; d;",
            "a abcabc abcabc "},
    };

    int e = sizeof(t) / sizeof(t[0]);
    int b = e, i;
    bool res = false;

    for(i = 0; i < b; i++) {
        res = false;
        repl(1, &res, t[i], &i);

        if(!res) {
            --e;
        }
    }

    if(e == b) {
        printf(ANSI_COLOR_GREEN "\n[PASSED] " ANSI_COLOR_RESET
            "REPL Test | %i out of %i passed\n\n", e, b);
    } else {
        printf(ANSI_COLOR_RED "\n[FAILED] " ANSI_COLOR_RESET
            "REPL Test | %i out of %i passed\n\n", e, b);
    }

}

void repl(bool is_test, bool * good, char ** input, int * j) {
    int i;
    char str[120], * file = NULL;
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
        printf("Type '\\h' for help and '\\q' to quit\n");
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
            } else if(strcmp(str, "\\o\n") == 0) {
                free(lexer);
                printf("*** Open file: ");
                fgets(str, 120, stdin);
                str[strlen(str) - 1] = '\0';

                if(strlen(str) > 0) {
                    file = read_file(str);

                    if(file != NULL) {
                        printf("File \"%s\" opened\n", str);
                        lexer = new_lexer(file);
                    } else {
                        printf("Bad file\n");
                        lexer = new_lexer("");
                    }
                } else {
                    lexer = new_lexer("");
                }
            } else if(strcmp(str, "\\h\n") == 0) {
                printf("Commands:\nDisplay environment: ... \\e\n");
                printf("Open file: ............. \\o\n");
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

        if(file != NULL) {
            free(file);
            file = NULL;
        }

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
