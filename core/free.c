/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * core/free.c
 *
 */

void free_index_expression(IndexExpression * ie) {
    ExpressionStatement * left = ie->left;
    ExpressionStatement * index = ie->index;

    //free_expression_statement(ie->left_expression_type, left->expression);
    free_expression_statement(ie->left_expression_type, left);
    free_expression_statement(ie->index_expression_type, index->expression);

    //free(ie->left_expression_type);
    free(ie->index_expression_type);

    //free(left);
    free(index);
    free(ie);
}

void free_array_literal(ArrayLiteral * al) {
    int i;
    Array * arr = al->elements;
    ExpressionStatement * est = NULL;

    for(i = 0; i < arr->size; i++) {
        est = arr->array[i];
        free_expression_statement(est->expression_type, est->expression);
        free(est->expression_type);
        //free(est);
    }

    array_free(arr);
    free(al);
}

void free_block_statement(BlockStatement * bst) {
    int i;

    for(i = 0; i < bst->sc; i++) {
        free_statement(bst->statements[i]);
    }

    free(bst->statements);
    free(bst);
}

void free_call_expression(CallExpression * call) {
    int i;

    free_expression_statement(call->function_type, call->function);

    for(i = 0; i < call->ac; i++) {
        free_expression_statement(call->arguments[i]->expression_type,
            call->arguments[i]->expression);
        free(call->arguments[i]->expression_type);
        free(call->arguments[i]);
    }

    if(call->ac > 0) {
        free(call->arguments);
    }

    free(call);
}

void free_function_literal(FunctionLiteral * fl) {
    int i;

    free_block_statement(fl->body);
    free(fl->token.literal);

    for(i = 0; i < fl->pc; i++) {
        free_identifier(fl->parameters[i]);
    }

    if(fl->pc > 0) {
        free(fl->parameters);
    }

    free(fl);
}

void free_if_expression(IfExpression * ifx) {
    free_expression_statement(ifx->condition_type, ifx->condition);

    free_block_statement(ifx->consequence);

    if(ifx->else_token.literal != NULL) {
        free(ifx->else_token.literal);
        free_block_statement(ifx->alternative);
    }

    free(ifx->condition_type);
    free(ifx->token.literal);
    free(ifx);
}

void free_boolean(Boolean * b) {
    free(b->token.literal);
    free(b);
}

void free_infix_expression(InfixExpression * iex) {
    free_expression_statement(iex->left_expression_type, iex->left);
    free_expression_statement(iex->right_expression_type, iex->right);
    free(iex->right_expression_type);
    free(iex);
}

void free_prefix_expression(PrefixExpression * pex) {
    free_expression_statement(pex->expression_type, pex->right);
    free(pex->expression_type);
    free(pex);
}

void free_identifier(Identifier * id) {
    free(id->token.literal);
    free(id);
}

void free_string_literal(StringLiteral * sl) {
    string_free(sl->value);
    free(sl);
}

void free_integer_literal(IntegerLiteral * il) {
    free(il->token.literal);
    free(il);
}

void free_expression_statement(char * type, void * value) {
    if(strcmp(type, INT) == 0) {
        free_integer_literal((IntegerLiteral *) value);
    } else if(strcmp(type, STRING) == 0) {
        free_string_literal((StringLiteral *) value);
    } else if (strcmp(type, IDENT) == 0) {
        free_identifier((Identifier *) value);
    } else if (strcmp(type, PREFIX) == 0) {
        free_prefix_expression((PrefixExpression *) value);
    } else if (strcmp(type, INFIX) == 0) {
        free_infix_expression((InfixExpression *) value);
    } else if (strcmp(type, TRUE) == 0 || strcmp(type, FALSE) == 0) {
        free_boolean((Boolean *) value);
    } else if(strcmp(type, IF) == 0) {
        free_if_expression((IfExpression *) value);
    } else if(strcmp(type, CALL) == 0) {
        free_call_expression((CallExpression *) value);
    } else if(strcmp(type, ARRAY) == 0) {
        free_array_literal((ArrayLiteral *) value);
    } else if(strcmp(type, ARRAYIDX) == 0) {
        free_index_expression((IndexExpression *) value);
    }
}

void free_return_statement(ReturnStatement * ret) {
    ExpressionStatement * es = (ExpressionStatement *) ret->value;

    free(ret->token.literal);
    free_expression_statement(ret->type, es->expression);
    free(ret->type);
    free(es);
}

void free_let_statement(LetStatement * let) {
    ExpressionStatement * es = (ExpressionStatement *) let->value;

    free(let->token.literal);
    free(let->name.value);
    free_expression_statement(let->type, es->expression);
    free(let->type);
    free(es);
}

void free_statement(Statement stmt) {
    LetStatement * let;
    ReturnStatement * ret;
    ExpressionStatement * es;

    if(stmt.type == LET) {
        let = (LetStatement *) stmt.st;
        free_let_statement(let);
        free(let);
    } else if(stmt.type == RETURN) {
        ret = (ReturnStatement *) stmt.st;
        free_return_statement(ret);
        free(ret);
    } else if(stmt.type == EXPRESSION) {
        es = (ExpressionStatement *) stmt.st;
        free_expression_statement(es->expression_type, es->expression);
        free(es->expression_type);
        free(es);
    }
}

void free_program(Lexer * lexer, Parser * parser, Program * program) {
    int i;

    for(i = 0; i < program->sc; i++) {
        free_statement(program->statements[i]);
    }

    for(int i = 0; i < parser->ec; i++) {
        free(parser->errors[i]);
    }

    free(parser->errors);
    free(parser);
    free(program->statements);
    free(lexer);
    free(program);
}
