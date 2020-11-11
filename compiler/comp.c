/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * compiler/comp.c
 *
 */

#include "types.c"
#include "common.c"
#include "compile.c"

#include "test/integerarith.c"
#include "test/insstring.c"
#include "test/readoperands.c"
#include "test/vm.c"

void comp_test() {
    init_definitions();
    comp_new_definition(OP_CONSTANT);
    comp_new_definition(OP_ADD);
    //SortedList * m = hash_map_find(definitions, "0");
    //test_integer_arithmetic();
    //test_instruction_string();
    //test_read_operands();
    test_vm();
    free_definitions();
}
