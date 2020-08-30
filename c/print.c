/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 */

char * print_call_expression(CallExpression * ce) {
    int i;

    ExpressionStatement * es;

    char * fnc = get_print_expression(ce->function_type, ce->function);
    char * fin = malloc(sizeof(char) * strlen(fnc));
    char * args = malloc(sizeof(char));
    char * exp;

    fin[0] = '\0';
    args[0] = '\0';

    int c = 1;

    for(i = 0; i < ce->ac; i++) {
        es = ce->arguments[i];
        exp = get_print_expression(es->expression_type, es->expression);
        c += strlen(exp) + 2;
        args = realloc(args, c);
        strcat(args, exp);

        if(i != ce->ac - 1) {
            strcat(args, ", ");
        }
    }

    fin = realloc(fin, strlen(args) + strlen(fnc) + 4);

    strcat(fin, fnc);
    strcat(fin, " (");
    strcat(fin, args);
    strcat(fin, ")");

    free(args);

    return fin;
}

char * print_function_literal(FunctionLiteral * fl) {
    int i;

    Identifier * id;

    char * params = malloc(sizeof(char));
    char * fin = malloc(sizeof(char) + 4);
    char * block;

    int psz = 1;

    params[0] = '\0';
    fin[0] = '\0';

    for(i = 0; i < fl->pc; i++) {
        id = fl->parameters[i];

        psz += strlen(id->value) + 2;
        params = realloc(params, psz);
        strcat(params, id->value);

        if(i != fl->pc - 1) {
            strcat(params, ", ");
        }
    }

    block = print_block_statement(fl->body);

    strcat(fin, "fn (");
    fin = realloc(fin, strlen(params) + strlen(block) + 7);
    strcat(fin, params);
    strcat(fin, ") ");
    strcat(fin, block);

    free(params);

    return fin;
}

char * print_block_statement(BlockStatement * bst) {
    int i;

    Statement stmt;

    char * pt = malloc(sizeof(char *));
    char * ap = NULL;

    int len = 4;

    pt[0] = '\0';

    strcat(pt, "{ ");

    for(i = 0; i < bst->sc; i++) {
        ap = get_print_statement_type(bst->statements[i]);

        len += strlen(ap) + 1;
        pt = realloc(pt, len);

        strcat(pt, ap);
        strcat(pt, " ");

        free(ap);
    }

    strcat(pt, "}");

    return pt;
}

char * print_if_expression(IfExpression * iex) {
    char * cond = get_print_expression(iex->condition_type, iex->condition);

    char * cons = print_block_statement(iex->consequence);
    char * alt = NULL;

    char * pt = malloc(5 + strlen(cond) + strlen(cons));

    pt[0] = '\0';

    strcat(pt, "if");
    strcat(pt, " ");
    strcat(pt, cond);
    strcat(pt, " ");
    strcat(pt, cons);

    if(iex->alternative != NULL) {
        alt = print_block_statement(iex->alternative);
        pt = realloc(pt, 6 + strlen(alt) +  strlen(cond) + strlen(cons));

        strcat(pt, " ");
        strcat(pt, alt);
    }

    return pt;
}

char * get_print_expression(char * type, void * expr) {
    char * s;

    if(strcmp(type, INT) == 0) {
        s = print_integer_literal((IntegerLiteral *) expr);
    } else if (strcmp(type, IDENT) == 0) {
        s = print_identifier_value((Identifier *) expr);
    } else if (strcmp(type, PREFIX) == 0) {
        s = print_prefix_expression((PrefixExpression *) expr);
    } else if (strcmp(type, INFIX) == 0) {
        s = print_infix_expression((InfixExpression *) expr);
    } else if (strcmp(type, TRUE) == 0 || strcmp(type, FALSE) == 0) {
        s = print_boolean((Boolean *) expr);
    } else if(strcmp(type, IF) == 0) {
        s = print_if_expression((IfExpression *) expr);
    } else if(strcmp(type, FUNCTION) == 0) {
        s = print_function_literal((FunctionLiteral *) expr);
    } else if(strcmp(type, CALL) == 0) {
        s = print_call_expression((CallExpression *) expr);
    }

    return s;
}

char * print_let_statement(LetStatement * smt) {
    int v = (strlen(smt->token.literal) + strlen(smt->name.value) + 5);

    ExpressionStatement * est = NULL;

    char * ap;
    char * let = malloc(sizeof(char) * v);

    let[0] = '\0';

    strcat(let, smt->token.literal);
    strcat(let, " ");
    strcat(let, smt->name.value);
    strcat(let, " = ");

    if(smt->value != NULL) {
        est = (ExpressionStatement *) smt->value;
        ap = get_print_expression(est->expression_type, est->expression);
        let = realloc(let, sizeof(char) * (v + strlen(ap) + 1));
        strcat(let, ap);
    }

    strcat(let, ";");
    free(ap);

    return let;
}

char * print_return_statement(ReturnStatement * smt) {
    int v = (strlen(smt->token.literal) + 3);

    char * ret = malloc(sizeof(char) * v);
    ExpressionStatement * est = NULL;
    char * ap;

    ret[0] = '\0';

    strcat(ret, smt->token.literal);
    strcat(ret, " ");

    if(smt->value != NULL) {
        est = (ExpressionStatement *) smt->value;
        ap = get_print_expression(smt->type, est->expression);
        ret = realloc(ret, v + strlen(ap));
        strcat(ret, ap);
    }

    strcat(ret, ";");
    free(ap);

    return ret;
}

char * print_integer_literal(IntegerLiteral * il) {
    return il->token.literal;
}

char * print_boolean(Boolean * b) {
    return b->token.literal;
}

char * print_identifier_value(Identifier * i) {
    return i->value;
}

char * print_prefix_expression(PrefixExpression * pe) {
    char * ap = get_print_expression(pe->expression_type, pe->right);
    char * exp = malloc(strlen(ap) + strlen(pe->operator) + 3);

    sprintf(exp, "(%s%s)", pe->operator, ap);

    free(ap);

    return exp;
}

char * print_infix_expression(InfixExpression * ie) {
    char * l = get_print_expression(ie->left_expression_type, ie->left);
    char * r = get_print_expression(ie->right_expression_type, ie->right);
    char * exp = malloc(5 + strlen(ie->operator) + strlen(l) + strlen(r));

    sprintf(exp, "(%s %s %s)", l, ie->operator, r);

    free(l);
    free(r);

    return exp;
}

char * get_print_statement_type(Statement stmt) {
    ExpressionStatement * est = NULL;

    char * ap = malloc(sizeof(char *));
    char * type = NULL;

    ap[0] = '\0';

    if(stmt.type == LET) {
        ap = print_let_statement((LetStatement *) stmt.st);
    } else if(stmt.type == RETURN) {
        ap = print_return_statement((ReturnStatement *) stmt.st);
    } else if(stmt.type == EXPRESSION) {
        type = ((ExpressionStatement *) stmt.st)->expression_type;
        est = ((ExpressionStatement *) stmt.st)->expression;
        ap = get_print_expression(type, est);
    }

    return ap;
}

char * print_program(Program * program) {
    int i;

    char * pt = malloc(sizeof(char));
    char * ap = NULL;

    int len = 1;

    pt[0] = '\0';

    for(i = 0; i < program->sc; i++) {
        ap = get_print_statement_type(program->statements[i]);
        len += strlen(ap) + 1;
        pt = realloc(pt, len);

        strcat(pt, ap);
        strcat(pt, "\n");

        free(ap);
    }

    return pt;
}

void test_print_program() {
    int i;

    const char * input = " \
        if (x < y) { x } else { !!!!5 }; \
        if (x < y) { a } else { if (x < y) { x } else { !!!!5 } } \
        !-5 \
        fn(x, y, z) { z }(asd) \
        !!!4 \
        a + b / 4 \
        -(1 + 2) \
        let x = 1 * 2 * 3; \
        let a = a * 2 + 3; \
        let v = n \
        3 > 5 == false \
        false; \
        fn(a) { let a = 4; } \
        !true \
        !!asd \
        let a = asd(12 + 3, asd, 3); \
        return 500;";

    Lexer * lexer = new_lexer(input);
    Parser * parser = new_parser(lexer);
    Program * program = parse_program(parser);

    for(i = 0; i < parser->ec; i++) {
        printf("%s\n", parser->errors[i]);
    }

    printf("%s", print_program(program));
}
