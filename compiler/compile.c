/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * compiler/compile.c
 *
 */

int compiler_add_instruction(Compiler * compiler, Instruction * ins) {
    Instruction * comp_instruction = compiler->instructions;
    int i, j;
    int pos = comp_instruction->instruction_length;
    int append_len = ins->instruction_length;

    comp_instruction->instruction = realloc(comp_instruction->instruction,
        sizeof(byte) * (pos + append_len));

    for(i = pos, j = 0; i < pos + append_len; i++, j++) {
        comp_instruction->instruction[i] = ins->instruction[j];
        comp_instruction->instruction_length += 1;
    }

    free_instruction(ins);
}

int compile_emit(Compiler * compiler, byte op, int operands[]) {
    Instruction * ins = make(op, operands);
    return compiler_add_instruction(compiler, ins);
}

int compiler_add_constant(Compiler * compiler, void * constant) {
    int sz = sizeof(Object *) * (compiler->constant_count + 1);

    compiler->constants = realloc(compiler->constants, sz);
    compiler->constants[compiler->constant_count++] = constant;

    return compiler->constant_count - 1;
}

void * compile_expression(Compiler * compiler, char * type, void * expression) {
    if(strcmp(type, INFIX) == 0) {
        InfixExpression * iex = (InfixExpression *) expression;
        int a[0] = {};

        compile_expression(compiler, iex->left_expression_type, iex->left);
        compile_expression(compiler, iex->right_expression_type, iex->right);

        if(strcmp(iex->operator, "+") == 0) {
            compile_emit(compiler, OP_ADD, a);
        }
    } else if(strcmp(type, INT) == 0) {
        IntegerLiteral * il = (IntegerLiteral *) expression;
        Object * int_obj = eval_integer(il, NULL);
        int v[1] = {compiler_add_constant(compiler, int_obj)};

        compile_emit(compiler, OP_CONSTANT, v);
    }
}

void * compile(Compiler * compiler, Program * program) {
    int i;
    Statement statement;
    LetStatement * let = NULL;
    ReturnStatement * ret = NULL;
    ExpressionStatement * est = NULL;

    for(i = 0; i < program->sc; i++) {
        statement = program->statements[0];

        if(strcmp(statement.type, LET) == 0) {

        } else if(strcmp(statement.type, RETURN) == 0) {

        } else if(strcmp(statement.type, EXPRESSION) == 0) {
            est = (ExpressionStatement *) statement.st;
            compile_expression(compiler, est->expression_type, est->expression);
        }
    }
}
