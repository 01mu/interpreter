/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * header/functions.h
 *
 */

Token new_token(char * type);
void read_char(Lexer * lexer);
int str_to_int(char * str);
Lexer * new_lexer(const char * input);
char * get_substr(int position, int final_pos, const char * input);
bool is_letter(char ch);
bool is_digit(char ch);
char * read_identifier(Lexer * lexer);
char * read_number(Lexer * lexer);
char * lookup_ident(char * literal);
void skip_whitespace(Lexer * lexer);
char peek_char(Lexer * lexer);
Token lexer_next_token(Lexer * lexer);
void parser_next_token(Parser * parser);
Parser * new_parser(Lexer * lexer);
bool cur_token_is(Parser * parser, char * type);
bool peek_token_is(Parser * parser, char * type);
void error_invalid_syntax(Parser * parser, Token t);
void peek_error(Parser * parser, char * type);
bool expect_peek(Parser * parser, char * type);
bool check_parser_errors(Parser * parser);

void parse_statement(Parser * par, Statement * stmts, int sc, int sz);
void * parse_call_expression(Parser * par, char * type, void * function);
ExpressionStatement ** parse_call_arguments(Parser * par, int * ac);
void * parse_expression(Parser * par, int precedence, void * ex, int et);
void parse_let_statement(Parser * par, Statement * smt);
void parse_return_statement(Parser * par, Statement * smt);
void * parse_identifier(Parser * par);
void * parse_integer_literal(Parser * par);
int parser_get_precedence(Parser * par, int type);
void * parse_prefix_expression(Parser * par);
void * parse_infix_expression(Parser * par, void * left, char * left_type);
void * parse_boolean(Parser * par, bool value);
void * parse_grouped_expression(Parser * par);
void parse_expression_statement(Parser * par, Statement * smt);
void * parse_if_expression(Parser * par);
BlockStatement * parse_block_statement(Parser * parser);
Program * parse_program(Parser * parser);
Identifier ** parse_program_parameters(Parser * parser, int * c);
void * parse_function_literal(Parser * parser);

char * read_file(char * file);
char * print_call_expression(CallExpression * ce);
char * print_function_literal(FunctionLiteral * fl);
char * print_block_statement(BlockStatement * bst);
char * print_if_expression(IfExpression * iex);
char * get_print_expression(char * type, void * expr);
char * print_let_statement(LetStatement * smt);
char * print_return_statement(ReturnStatement * smt);
char * print_integer_literal(IntegerLiteral * il);
char * print_boolean(Boolean * b);
char * print_identifier_value(Identifier * i);
char * print_prefix_expression(PrefixExpression * pe);
char * print_infix_expression(InfixExpression * ie);
char * get_print_statement_type(Statement stmt);
char * print_program(Program * program);
void test_print_program();

void free_integer_literal(IntegerLiteral * il);
void free_identifier(Identifier * id);
void free_prefix_expression(PrefixExpression * pex);
void free_infix_expression(InfixExpression * iex);
void free_block_statement(BlockStatement * bst);
void free_if_expression(IfExpression * ifx);
void free_function_literal(FunctionLiteral * fl);
void free_call_expression(CallExpression * call);
void free_expression_statement(char * type, void * value);
void free_let_statement(LetStatement * let);
void free_return_statement(ReturnStatement * ret);
void free_statement(Statement stmt);
void free_program(Lexer * lexer, Parser * parser, Program * program);

void test_parse_all(char * option);
bool get_parse_test_expression(ExpressionStatement ** es, char * input);
char * test_get_statement_type(char * type, void * expr);
bool test_string_cmp(char * b, char * g, char * ex, int i, int * fail);
bool test_int_cmp(char * b, int g, int ex, int i, int * fail);
char * print_test_result(char * b, int fail, int tc);
char * test_next_token();
char * test_let_statements();
char * test_return_statements();
char * test_identifier_expression();
char * test_parsing_boolean_expression();
char * test_integer_literal_expression();
char * test_parsing_prefix_expressions();
char * test_parsing_infix_expressions();
char * test_parsing_function_literal_expressions();
char * test_parsing_call_expressions();
char * test_parsing_grouped_expressions();
char * test_parsing_if_expressions();

Env * new_env();
Object * env_get(Env * env, char * name);
Object * env_set(Env * env, char * name, Object * data);
void env_test();

bool is_bool_or_ident(char * t);
void free_eval_expression(char * ext, Object * obj, Env * env, bool free_obj);
bool is_error(Object * obj);
Object * new_error(char * msg);
void init_bool(Object ** b, bool lit);
void free_stat(Object * b);
char * inspect_error_object(ErrorObject * eobj);
char * inspect_null_object(NullObject * nobj);
int inspect_integer_object(IntegerObject * iobj);
bool inspect_boolean_object(BooleanObject * bobj);
String * print_object(Object * obj);
bool is_truthy(Object * obj);
Object * eval_integer(IntegerLiteral * il, Env * env);
Object * eval_bool(bool b, Env * env);
Object * eval_bang_operator_expression(Object * right, Env * env);
Object * eval_minus_prefix_operator_expression(Object * right, Env * env);
Object * eval_prefix_expression(PrefixExpression * pex, Env * env);
Object * eval_integer_infix_exp(char * op, Object * l, Object * r, Env * env);
Object * eval_infix_expression(InfixExpression * iex, Env * env);
Object * eval_if_expression(IfExpression * iex, Env * env);
Object * eval_identifier(Identifier * ident, Env * env);
Object ** eval_expressions(ExpressionStatement ** args, int c, Env * env);
Env * extend_function_env(Function * func, Object ** args);
Object * unwrap_return_value(Object * obj);
Object * apply_function(Object * obj, Object ** args, int argc);
Object * eval_call_expression(CallExpression * ce, Env * env);
Object * eval_expression(char * ext, void * est, Env * env);
Object * eval_return_statement(ExpressionStatement * est, Env * env);
Object * eval_let_statement(ExpressionStatement * est, Env * env, char * name);
Object * eval_statement(Statement statement, Env * env);
Object * eval_statements(Statement * statements, int sc, Env * env);

String * string_new();
void string_cat(String * string, char * ap, bool h);
void string_append(String * string, String * ap);
void string_free(String * string);

SortedList * sorted_list_new(void * data, char * key);
SortedList * sorted_list_ins(SortedList ** r, void * d, char * key);
bool sorted_list_remove(SortedList ** r, char * key);
SortedList * sorted_list_find(SortedList * r, char * key);
String * sorted_list_print(SortedList * sl);
void sorted_list_free(SortedList * sl);

HashMap * hash_map_new(int size);
void * hash_map_insert(HashMap * hm, char * key, void * data);
void hash_map_remove(HashMap * hm, char * key);
SortedList * hash_map_find(HashMap * hm, char * key);
int hash_map_hash(HashMap * hm, char * key);
String * hash_map_print(HashMap * hm);
void hash_map_free(HashMap * hm);
void hash_map_test();
