/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/repl.c
 *
 */

void repl();
void repl_do_test(char * input);
void repl_test();

void repl_test() {
    int t = 0;
    int i;

    if(t == 0) {
        char * t[] = {
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
            "let a = fn(x) { let z = 2; let a = z * 2; return a + x; }; \
                a(3) + 2;",
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
        };

        for(i = 0; i < sizeof(t) / sizeof(t[1]); i++) {
            repl_do_test(t[i]);
        }
    } else {
        char * t[] = {
            "let a = fn() { 55; }; a();",
        };

        for(i = 0; i < sizeof(t) / sizeof(t[1]); i++) {
            repl_do_test(t[i]);
        }
    }
}

void repl_do_test(char * input) {
    fls = malloc(sizeof(FunctionLiteralStore));
    fls->count = 0;
    fls->store = malloc(sizeof(FunctionLiteral *));

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Env * env = new_env();

    if(check_parser_errors(parser)) {
        env_free(env);
        free_program(lexer, parser, program);
        return;
    }

    if(program->sc > 0) {
        eval_statements(program->statements, program->sc, env);
    }

    if(out != NULL) {
        env_free(out);
        out = NULL;
    }

    free_program(lexer, parser, program);

    for(int i = 0; i < fls->count; i++) {
        free_function_literal((FunctionLiteral *) fls->store[i]);
    }

    free(fls->store);
    free(fls);
    env_free(env);
}

void repl() {
    Lexer * lexer = NULL;
    Parser * parser = NULL;
    Program * program = NULL;
    Env * env = new_env();

    char str[120];

    printf("Type '\\h' for help and '\\q' to quit.\n");

    fls = malloc(sizeof(FunctionLiteralStore));
    fls->count = 0;
    fls->store = malloc(sizeof(FunctionLiteral *));

    while(1) {
        printf(">>> ");
        fgets(str, 120, stdin);

        if(strcmp(str, "\\q\n") == 0) {
            printf("Bye!\n");
            break;
        } else if(strcmp(str, "\\e\n") == 0) {
            env_display(env);
            continue;
        } else if(strcmp(str, "\\h\n") == 0) {
            printf("Commands:\nDisplay environment: \\e\n");
            printf(">>> ");
            fgets(str, 120, stdin);
        }

        lexer = new_lexer(str);
        parser = new_parser(lexer);
        program = parse_program(parser);

        if(check_parser_errors(parser)) {
            free_program(lexer, parser, program);
            continue;
        }

        if(program->sc > 0) {
            eval_statements(program->statements, program->sc, env);
        }

        if(out != NULL) {
            env_free(out);
            out = NULL;
        }

        free_program(lexer, parser, program);
    }

    for(int i = 0; i < fls->count; i++) {
        free_function_literal((FunctionLiteral *) fls->store[i]);
    }

    free(fls->store);
    free(fls);
    env_free(env);
}
