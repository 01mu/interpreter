/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/print.c
 *
 */

char * print_block_statement(BlockStatement * bst) {
    int i;
    Statement stmt;
    char * pt = malloc(sizeof(char) * 3), * ap = NULL;
    size_t stmt_sz = 4;

    pt[0] = '\0';
    strcat(pt, "{ ");

    for(i = 0; i < bst->sc; i++) {
        ap = get_print_statement_type(bst->statements[i]);
        stmt_sz += strlen(ap) + 1;
        pt = realloc(pt, stmt_sz);
        strcat(pt, ap);
        strcat(pt, " ");
        free(ap);
    }

    strcat(pt, "}");
    return pt;
}

char * print_call_expression(CallExpression * ce) {
    int i;
    ExpressionStatement * es = NULL;
    char * fnc = get_print_expression(ce->function_type, ce->function);
    char * fin = malloc(sizeof(char) * strlen(fnc));
    char * args = malloc(sizeof(char)), * exp = NULL;
    size_t arg_sz = 1;

    fin[0] = '\0';
    args[0] = '\0';

    for(i = 0; i < ce->ac; i++) {
        es = ce->arguments[i];
        exp = get_print_expression(es->expression_type, es->expression);
        arg_sz += strlen(exp) + 2;
        args = realloc(args, arg_sz);
        strcat(args, exp);
        free(exp);

        if(i != ce->ac - 1) {
            strcat(args, ", ");
        }
    }

    fin = realloc(fin, sizeof(char) * (strlen(args) + strlen(fnc) + 4));
    strcat(fin, fnc);
    strcat(fin, " (");
    strcat(fin, args);
    strcat(fin, ")");
    free(args);
    free(fnc);
    return fin;
}

char * print_function_literal(FunctionLiteral * fl) {
    int i;
    Identifier * id = NULL;
    char * params = malloc(sizeof(char)), * fin = malloc(sizeof(char));
    char * block = print_block_statement(fl->body);
    size_t param_sz = 1;

    params[0] = '\0';
    fin[0] = '\0';

    for(i = 0; i < fl->pc; i++) {
        id = fl->parameters[i];
        param_sz += strlen(id->value) + 2;
        params = realloc(params, param_sz);
        strcat(params, id->value);

        if(i != fl->pc - 1) {
            strcat(params, ", ");
        }
    }

    fin = realloc(fin, sizeof(char) * (strlen(params) + strlen(block) + 7));
    strcat(fin, "fn (");
    strcat(fin, params);
    strcat(fin, ") ");
    strcat(fin, block);
    free(params);
    free(block);
    return fin;
}

char * print_if_expression(IfExpression * iex) {
    char * cond = get_print_expression(iex->condition_type, iex->condition);
    char * cons = print_block_statement(iex->consequence);
    char * pt = malloc(5 + strlen(cond) + strlen(cons)), * alt = NULL;
    size_t alt_sz;

    pt[0] = '\0';
    strcat(pt, "if");
    strcat(pt, " ");
    strcat(pt, cond);
    strcat(pt, " ");
    strcat(pt, cons);

    if(iex->alternative != NULL) {
        alt = print_block_statement(iex->alternative);
        alt_sz = 6 + strlen(alt) +  strlen(cond) + strlen(cons);
        pt = realloc(pt, sizeof(char) * alt_sz);
        strcat(pt, " ");
        strcat(pt, alt);
        free(alt);
    }

    free(cond);
    free(cons);
    return pt;
}

char * print_boolean(Boolean * b) {
    char * lit = malloc(sizeof(char) * (strlen(b->token.literal) + 1));
    strcpy(lit, b->token.literal);
    return lit;
}

char * print_infix_expression(InfixExpression * ie) {
    char * l = get_print_expression(ie->left_expression_type, ie->left);
    char * r = get_print_expression(ie->right_expression_type, ie->right);
    size_t sz = 5 + strlen(ie->operator) + strlen(l) + strlen(r);
    char * exp = malloc(sizeof(char) * sz);

    sprintf(exp, "(%s %s %s)", l, ie->operator, r);
    free(l);
    free(r);
    return exp;
}

char * print_prefix_expression(PrefixExpression * pe) {
    char * ap = get_print_expression(pe->expression_type, pe->right);
    char * exp = malloc(strlen(ap) + strlen(pe->operator) + 3);

    sprintf(exp, "(%s%s)", pe->operator, ap);
    free(ap);
    return exp;
}

char * print_identifier_value(Identifier * i) {
    char * lit = malloc(sizeof(char) * (strlen(i->value) + 1));
    strcpy(lit, i->value);
    return lit;
}

char * print_integer_literal(IntegerLiteral * il) {
    char * lit = malloc(sizeof(char) * (strlen(il->token.literal) + 1));
    strcpy(lit, il->token.literal);
    return lit;
}

char * get_print_expression(char * type, void * expr) {
    if(strcmp(type, INT) == 0) {
        return print_integer_literal((IntegerLiteral *) expr);
    } else if(strcmp(type, IDENT) == 0) {
        return print_identifier_value((Identifier *) expr);
    } else if(strcmp(type, PREFIX) == 0) {
        return print_prefix_expression((PrefixExpression *) expr);
    } else if(strcmp(type, INFIX) == 0) {
        return print_infix_expression((InfixExpression *) expr);
    } else if(strcmp(type, TRUE) == 0 || strcmp(type, FALSE) == 0) {
        return print_boolean((Boolean *) expr);
    } else if(strcmp(type, IF) == 0) {
        return print_if_expression((IfExpression *) expr);
    } else if(strcmp(type, FUNCTION) == 0) {
        return print_function_literal((FunctionLiteral *) expr);
    } else if(strcmp(type, CALL) == 0) {
        return print_call_expression((CallExpression *) expr);
    }
}

char * print_return_statement(ReturnStatement * smt) {
    ExpressionStatement * est = NULL;
    size_t sz = strlen(smt->token.literal) + 3;
    char * ret = malloc(sizeof(char) * sz), * ap = NULL;

    ret[0] = '\0';
    strcat(ret, smt->token.literal);
    strcat(ret, " ");

    if(smt->value != NULL) {
        est = (ExpressionStatement *) smt->value;
        ap = get_print_expression(smt->type, est->expression);
        ret = realloc(ret, sz + strlen(ap));
        strcat(ret, ap);
        free(ap);
    }

    strcat(ret, ";");
    return ret;
}

char * print_let_statement(LetStatement * smt) {
    ExpressionStatement * est = NULL;
    size_t sz = strlen(smt->token.literal) + strlen(smt->name.value) + 5;
    char * let = malloc(sizeof(char) * sz), * ap = NULL;

    let[0] = '\0';
    strcat(let, smt->token.literal);
    strcat(let, " ");
    strcat(let, smt->name.value);
    strcat(let, " = ");

    if(smt->value != NULL) {
        est = (ExpressionStatement *) smt->value;
        ap = get_print_expression(est->expression_type, est->expression);
        let = realloc(let, sizeof(char) * (sz + strlen(ap) + 1));
        strcat(let, ap);
        free(ap);
    }

    strcat(let, ";");
    return let;
}

char * get_print_statement_type(Statement stmt) {
    ExpressionStatement * est = NULL;
    char * type = NULL;

    if(stmt.type == LET) {
        return print_let_statement((LetStatement *) stmt.st);
    } else if(stmt.type == RETURN) {
        return print_return_statement((ReturnStatement *) stmt.st);
    } else if(stmt.type == EXPRESSION) {
        type = ((ExpressionStatement *) stmt.st)->expression_type;
        est = ((ExpressionStatement *) stmt.st)->expression;
        return get_print_expression(type, est);
    }
}

char * print_program(Program * program) {
    int i, len = 1;
    char * final = malloc(sizeof(char)), * stmt = NULL;

    final[0] = '\0';

    for(i = 0; i < program->sc; i++) {
        stmt = get_print_statement_type(program->statements[i]);
        len += strlen(stmt) + 1;
        final = realloc(final, len);
        strcat(final, stmt);
        strcat(final, "\n");
        free(stmt);
    }

    return final;
}

char * read_file(char * file) {
   char * buffer = NULL;
   int string_size, read_size;
   FILE * handler = fopen(file, "r");

   if(handler) {
       fseek(handler, 0, SEEK_END);
       string_size = ftell(handler);
       rewind(handler);
       buffer = (char *) malloc(sizeof(char) * (string_size + 1));
       read_size = fread(buffer, sizeof(char), string_size, handler);
       buffer[string_size] = '\0';

       if(string_size != read_size) {
           free(buffer);
           buffer = NULL;
       }

       fclose(handler);
    }

    return buffer;
}

void test_print_program() {
    int i;
    char * print;
    const char * input = " \
        if (x < y) { x let a = b; z; z z } else { !!!!5 } \
        if (x < y) { a } else { if (x < y) { x } else { !!!!5 } } \
        !-5 \
        fn(x, y, z) { z }(asd) \
        asd(a, b)(a, b) \
        !!!4 \
        asdad12 \
        -4 \
        a + b / 4 \
        -(1 + 2) \
        let x = 1 * 2 * 3; \
        let a = a * 2 + 3; \
        let v = n; \
        3 > 5 == false \
        false; \
        fn(a) { let a = 4; } \
        !true \
        !!asd \
        let a = asd(12 + 3, asd, 3); \
        return 500;";

    char * file = read_file("input");

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);

    printf("%i statements\n", program->sc);
    print = print_program(program);
    //eval_statement(program->statements[0]);
    printf("%s", print);
    free(print);
    free(file);
    free_program(lexer, parser, program);
}
