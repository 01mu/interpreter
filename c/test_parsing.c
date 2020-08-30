/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 */

char * test_get_statement_type(char * type, void * expr) {
    if(strcmp(type, LET) == 0) {
        return ((LetStatement *) expr)->type;
    } else if(strcmp(type, RETURN) == 0) {
        return ((ReturnStatement *) expr)->type;
    } else {
        return ((ExpressionStatement *) expr)->expression_type;
    }
}

bool test_expected_type(char * g, char * ex, int i, int * fail) {
    if(strcmp(g, ex) != 0) {
        printf("[Error %i] Expected type %s got %s\n", i, ex, g);
        * fail += 1;

        return false;
    }

    return true;
}

void test_next_token() {
    int i;

    const char * input =
        "let x = 5; \
        let y = 10; \
        let foobar = 838383;";

    const char * tests[16][2] = {
        {LET, "let"},
        {IDENT, "x"},
        {ASSIGN, "="},
        {INT, "5"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "y"},
        {ASSIGN, "="},
        {INT, "10"},
        {SEMICOLON, ";"},
        {LET, "let"},
        {IDENT, "foobar"},
        {ASSIGN, "="},
        {INT, "838383"},
        {SEMICOLON, ";"},
        {EOF_, ""}};

    Lexer * lexer = new_lexer(input);
    Token token = lexer_next_token(lexer);

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        printf("%s %s '%c' %i %i \n", token.literal, token.type, lexer->ch,
            lexer->pos, lexer->read_pos);

        if(strcmp(token.type, tests[i][0]) != 0)
            printf("[%i] Token type wrong: expected %s got %s \n",
                i, tests[i][0], token.type);

        if(strcmp(token.literal, tests[i][1]) != 0)
            printf("[%i] Token literal wrong: expected %s got %s \n",
                i, tests[i][1], token.literal);

        if(token.type == LET || token.type == FUNCTION
            || token.type == IDENT || token.type == INT)
            free(token.literal);

        token = lexer_next_token(lexer);
    }

    free(lexer);
}

void test_let_statements() {
    int i;

    const char * input = " \
            let x = 5; \
            let y = 10; \
            let foobar = 838383;";

    const char * tests[3] = {"x", "y", "foobar"};

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);
    Statement stmt;
    LetStatement * ls;
    const char * name;

    if(check_parser_errors(parser))
        return;

    if(program == NULL)
        printf("parse_program() returned NULL");

    if(program->sc != 3)
        printf("Expected 3 statements, got %i", program->sc);

    for(i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        stmt = program->statements[i];
        name = tests[i];
        ls = (LetStatement *) stmt.st;

        if(strcmp(stmt.type, LET) != 0)
            printf("Statement's type not \"LET\", got \"%s\"\n", stmt.type);

        if(strcmp(ls->token.type, LET) != 0)
            printf("Token's type not \"LET\", got \"%s\"\n", ls->token.type);

        if(strcmp(ls->name.value, name) != 0)
            printf("Name's value not '%s', got '%s'\n", name, ls->name.value);

        if(strcmp(ls->name.token.literal, name) != 0)
            printf("Name's token literal not '%s', got '%s'\n", name,
                ls->name.token.literal);
    }
}

void test_return_statements() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        char * type;
    } t[5] = {
        {"return x;", IDENT},
        {"return 5 + 5;", INFIX},
        {"return 5;", INT},
        {"return if (x) { } else { };", IF},
        {"return !2;", PREFIX}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ReturnStatement * ret;

    printf("Testing RETURN statements\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        ret = (ReturnStatement *) stmt.st;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(!test_expected_type(stmt.type, RETURN, i, &fail)) {
            continue;
        }

        printf("%s\n", print_return_statement(ret));

        if(strcmp(t[i].type, ret->type) != 0) {
            fail++;
            printf("[Error %i] Expected return type %s got %s\n",
                i, t[i].type, ret->type);
        }
    }

    printf("RETURN: %i out of %i tests passed\n", tc - fail, tc);
}

void test_identifier_expression() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        char * expected;
    } t[5] = {
        {"zz_zz;", "zz_zz"},
        {"a ;", "a"},
        {"bbb ;", "bbb"},
        {"meme;", "meme"},
        {"foobar ;;;", "foobar"}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    Identifier * id;

    printf("Testing IDENTIFIER expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        id = (Identifier *) es->expression;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(!test_expected_type(es->expression_type, IDENT, i, &fail)) {
            continue;
        }

        printf("%s\n", print_identifier_value(id));

        if(strcmp(id->value, t[i].expected) != 0) {
            fail++;
            printf("[Error %i] Expected identifier value %s got %s\n",
                i, t[i].expected, id->value);
        }
    }

    printf("IDENTIFIER: %i out of %i tests passed\n", tc - fail, tc);
}

void test_integer_literal_expression() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        int value;
    } t[5] = {
        {"5;", 5},
        {"11111;", 11111},
        {"2;", 2},
        {"0;", 0},
        {"100000;", 100000}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    IntegerLiteral * il;

    printf("Testing INTEGER expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        il = (IntegerLiteral *) es->expression;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(!test_expected_type(es->expression_type, INT, i, &fail)) {
            continue;
        }

        printf("%s\n", print_integer_literal(il));

        if(il->value != t[i].value) {
            fail++;
            printf("[Error %i] Expected integer value %i got %i\n",
                i, t[i].value, il->value);
        }
    }

    printf("INTEGER: %i out of %i tests passed\n", tc - fail, tc);
}

void test_parsing_prefix_expressions() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        char * operator;
        char * expression_type;
    } tests[5] = {
        {"!5", "!", INT},
        {"-15", "-", INT},
        {"!fn(x) { } ", "!", FUNCTION},
        {"-var", "-", IDENT},
        {"!if(a) { } else { }", "!", IF}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    PrefixExpression * pex;

    printf("Testing PREFIX expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(tests[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        pex = (PrefixExpression *) es->expression;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(!test_expected_type(es->expression_type, PREFIX, i, &fail)) {
            continue;
        }

        printf("%s\n", print_prefix_expression(pex));

        if(strcmp(pex->operator, tests[i].operator) != 0) {
            fail++;
            printf("[Error: %i] Expected operator %s got %s\n",
                i, tests[i].operator, pex->operator);
        } else if(strcmp(pex->expression_type, tests[i].expression_type) != 0) {
            fail++;
            printf("[Error: %i] Expected right value %s got %s\n",
                i, tests[i].expression_type, pex->expression_type);
        }
    }

    printf("PREFIX: %i out of %i tests passed\n", tc - fail, tc);
}

void test_parsing_infix_expressions() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        int left_val;
        char * operator;
        int right_val;
    } t[5] = {
        {"100 + 1;", 100, "+", 1},
        {"0 - 1;", 0, "-", 1},
        {"2 * 23;", 2, "*", 23},
        {"7 / 51;", 7, "/", 51},
        {"2 != 3;", 2, "!=", 3}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    InfixExpression * iex;
    IntegerLiteral * right_il;
    IntegerLiteral * left_il;

    printf("Testing INFIX expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        iex = (InfixExpression *) es->expression;
        right_il = (IntegerLiteral *) iex->right;
        left_il = (IntegerLiteral *) iex->left;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(!test_expected_type(es->expression_type, INFIX, i, &fail)) {
            continue;
        }

        printf("%s\n", print_infix_expression(iex));

        if(strcmp(iex->operator, t[i].operator) != 0) {
            fail++;
            printf("[Error: %i] Expected operator %s got %s\n",
                i, t[i].operator, iex->operator);
        } else if(left_il->value != t[i].left_val) {
            fail++;
            printf("[Error: %i] Expected left value %i got %i\n",
                i, t[i].left_val, left_il->value);
        } else if(right_il->value != t[i].right_val) {
            fail++;
            printf("[Error: %i] Expected right value %i got %i\n",
                i, t[i].right_val, right_il->value);
        }
    }

    printf("INFIX: %i out of %i tests passed\n", tc - fail, tc);
}

void test_parsing_function_literal_expressions() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        int param_count;
        int statement_count;
    } t[5] = {
        {"fn(a) { let a = 4; }", 1, 1},
        {"fn(a, b) { x a }", 2, 2},
        {"fn(z) { !5 }", 1, 1},
        {"fn(a) { }", 1, 0},
        {"fn() { }", 0, 0}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    FunctionLiteral * fl;
    BlockStatement * bs;

    printf("Testing FUNCTION LITERAL expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        fl = (FunctionLiteral *) es->expression;
        bs = (BlockStatement *) fl->body;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(!test_expected_type(es->expression_type, FUNCTION, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_function_literal(fl));

        if(fl->pc != t[i].param_count) {
            fail++;
            printf("[Error: %i] Expected parameter count %i got %i\n",
                i, t[i].param_count, fl->pc);
        } else if(bs->sc != t[i].statement_count) {
            fail++;
            printf("[Error: %i] Expected statement count %i got %i\n",
                i, t[i].statement_count, bs->sc);
        }
    }

    printf("FUNCTION LITERAL: %i out of %i tests passed\n", tc - fail, tc);
}

void test_parsing_call_expressions() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        char * function_type;
        int ac;
        char * arg_type;
    } t[5] = {
        {"t(if(a > u) { a } else { return u; }, 2, 5)", IDENT, 3, IF},
        {"t(!asd)", IDENT, 1, PREFIX},
        {"func(false, 2, false)", IDENT, 3, FALSE},
        {"fn(1, 2, 3) { } ( 3 )", FUNCTION, 1, INT},
        {"fn(z) { z } (var)", FUNCTION, 1, IDENT}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    CallExpression * cex;

    printf("Testing CALL expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        cex = (CallExpression *) es->expression;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(!test_expected_type(es->expression_type, CALL, i, &fail)) {
            continue;
        }

        printf("[%i] %s\n", i, print_call_expression(cex));

        if(strcmp(cex->function_type, t[i].function_type) != 0) {
            fail++;
            printf("[Error: %i] Expected function type %s got %s\n",
                i, t[i].function_type, cex->function_type);
        } else if(cex->ac != t[i].ac) {
            fail++;
            printf("[Error: %i] Expected argument count %i got %i\n",
            i, t[i].ac, cex->ac);
        }

        es = (ExpressionStatement *) cex->arguments[0];

        if(strcmp(es->expression_type, t[i].arg_type) != 0) {
            fail++;
            printf("[Error: %i] Expected first argument type %s got %s\n",
                i, t[i].arg_type, es->expression_type);
        }
    }

    printf("CALL: %i out of %i tests passed\n", tc - fail, tc);
}

void test_parsing_grouped_expressions() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        char * group_type;
        char * lt;
        char * rt;
    } t[5] = {
        {"1 + (2 + 3) + var", INFIX, INFIX, IDENT},
        {"(5 + 5) * 2", INFIX, INFIX, INT},
        {"2 / (5 + 5)", INFIX, INT, INFIX},
        {"-(5 + 5)", PREFIX, MINUS, INFIX},
        {"!(true == false)", PREFIX, BANG, INFIX}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    PrefixExpression * pex;
    InfixExpression * iex;

    printf("Testing GROUPED expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(strcmp(es->expression_type, INFIX) == 0) {
            iex = (InfixExpression *) es->expression;
            printf("[%i] %s\n", i, print_infix_expression(iex));

            if(strcmp(iex->left_expression_type, t[i].lt) != 0) {
                fail++;
                printf("[Error: %i] Expected left type %s got %s\n",
                    i, t[i].lt, iex->left_expression_type);
            } else if(strcmp(iex->right_expression_type, t[i].rt) != 0) {
                fail++;
                printf("[Error: %i] Expected right type %s got %s\n",
                    i, t[i].rt, iex->right_expression_type);
            }
        } else if(strcmp(es->expression_type, PREFIX) == 0){
            pex = (PrefixExpression *) es->expression;
            printf("[%i] %s\n", i, print_prefix_expression(pex));

            if(strcmp(pex->operator, t[i].lt) != 0) {
                fail++;
                printf("[Error: %i] Expected left type %s got %s\n",
                    i, t[i].lt, pex->operator);
            } else if(strcmp(pex->expression_type, t[i].rt) != 0) {
                fail++;
                printf("[Error: %i] Expected right type %s got %s\n",
                    i, t[i].rt, pex->expression_type);
            }
        }
    }

    printf("GROUPED: %i out of %i tests passed\n", tc - fail, tc);
}

void test_parsing_if_expressions() {
    int i;
    int tc = 5;
    int fail = 0;

    struct {
        char * input;
        char * condition_type;;
        char * cons_type;
        char * alt_type;
    } t[5] = {
        {"if (x) { let a = 3; } else { return 1; }",
            IDENT, INT, INT},
        {"if (x == 1) { let a = b; } else { 5 }",
            INFIX, IDENT, INT},
        {"if (x) { let a = b; } else { 5 }",
            IDENT, IDENT, INT},
        {"if (x) { let a = b; } else { if (z) { } else { } }",
            IDENT, IDENT, IF},
        {"if (a > 3) { !2 } else { false }",
            INFIX, PREFIX, FALSE}};

    Lexer * lexer;
    Parser * parser;
    Program * program;
    Statement stmt;
    ExpressionStatement * es;
    IfExpression * ifx;
    BlockStatement * cons;
    BlockStatement * alt;

    char * cons_type;
    char * alt_type;

    printf("Testing IF expressions\n");

    for(i = 0; i < tc; i++) {
        lexer = new_lexer(t[i].input);
        parser = new_parser(lexer);
        program = parse_program(parser);
        stmt = program->statements[0];
        es = (ExpressionStatement *) stmt.st;
        ifx = (IfExpression *) es->expression;
        cons = (BlockStatement *) ifx->consequence;
        alt = (BlockStatement *) ifx->alternative;

        if(check_parser_errors(parser)) {
            fail++;
            continue;
        }

        if(!test_expected_type(es->expression_type, IF, i, &fail)) {
            continue;
        }

        cons_type = test_get_statement_type(cons->statements[0].type,
            cons->statements[0].st);
        alt_type = test_get_statement_type(alt->statements[0].type,
            alt->statements[0].st);

        printf("[%i] %s\n", i, print_if_expression(ifx));

        if(strcmp(ifx->condition_type, t[i].condition_type) != 0) {
            fail++;
            printf("[Error: %i] Expected condition type %s got %s\n",
                i, t[i].condition_type, ifx->condition_type);
        } else if(strcmp(cons_type, t[i].cons_type) != 0) {
            fail++;
            printf("[Error: %i] Expected consequence type %s got %s\n",
                i, t[i].cons_type, cons_type);
        } else if(strcmp(alt_type, t[i].alt_type) != 0) {
            fail++;
            printf("[Error: %i] Expected altnerative type %s got %s\n",
                i, t[i].alt_type, alt_type);
        }
    }

    printf("IF: %i out of %i tests passed\n", tc - fail, tc);
}
