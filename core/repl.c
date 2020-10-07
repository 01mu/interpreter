/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/repl.c
 *
 */

void repl(bool is_test, bool * good, char ** input, int * j);
void init_function_literal_store();
void init_env_store();
void free_function_literal_store();
void free_env_store();
void create_fn_map();

void repl(bool is_test, bool * good, char ** input, int * j) {
    int i;
    char str[120], * file = NULL;
    Lexer * lexer = NULL;
    Parser * parser = NULL;
    Program * program = NULL;
    Env * env = new_env();

    init_function_literal_store();
    init_env_store();

    if(is_test) {
        repl_test_string = string_new();
        is_repl_test_string = true;
    }

    eval_env_store_add(env);
    create_fn_map();

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
    hash_map_free(fn_map);

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

void init_function_literal_store() {
    fls = malloc(sizeof(FunctionLiteralStore));
    fls->count = 0;
    fls->store = malloc(sizeof(FunctionLiteral *));
}

void init_env_store() {
    env_store = malloc(sizeof(EnvStore));
    env_store->count = 0;
    env_store->store = malloc(sizeof(Env *));
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

void create_fn_map() {
    char * t[] = {"len", "find", "str", "push"};
    int i, c = sizeof(t) / sizeof(t[0]);
    char * z[c];

    fn_map = hash_map_new(26);

    for(i = 0; i < c; i++) {
        z[i] = malloc(strlen(t[i]) + 1);
        z[i][0] = '\0';
        strcat(z[i], t[i]);
        hash_map_insert(fn_map, z[i], NULL);
    }
}
