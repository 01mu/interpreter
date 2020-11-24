# interpreter
A C implementation of the [Monkey](https://monkeylang.org/) programming language defined in Thorsten Ball's book ["Writing A Interpreter In Go"](https://interpreterbook.com/).
## Compiling and testing
### Compile without tests
`gcc -o monkey main.c`
### Compile with tests
Compile the interpreter and perform tests of the different statements and expressions available in `tests/repl.c`.

`gcc -o monkey main.c; valgrind --leak-check=full --show-leak-kinds=all ./monkey test`
```
(...)
==5256== Command: ./monkey test
==5256==

[PASSED] REPL Test | 90 out of 90 passed

==5256==
==5256== HEAP SUMMARY:
==5256==     in use at exit: 0 bytes in 0 blocks
==5256==   total heap usage: 13,549 allocs, 13,549 frees, 293,249 bytes allocated
==5256==
==5256== All heap blocks were free
d -- no leaks are possible
(...)
```
## Types and control flow
The language currently supports closures, conditional statements, boolean expressions, integers, strings, arrays, and hash maps. Prefix and infix operations are available for some types.

The identifiers `let` and `return` define `statements` and do not produce values. The aforementioned type are called `expressions` since they produce values. For example, `let x = 5` does not produce a value, but `5` does.

When the code is parsed and evaluated, `x` will refer to the name of an `Object` type containing a reference to an `IntegerObject` type containing the integer literal value of `5`.
## Running code
### Execute REPL
Access the REPL by providing `repl` as an argument as in `./monkey repl`.
#### Output
```
Type '\h' for help and '\q' to quit
>>> let a = 5;
5
>>> a + 10;
15
>>> \h
Commands:
Display environment: ... \e
Open file: ............. \o
>>> \e
[10] a:  0x557f0a1bd290,
>>> \q
Bye!
```
### Open a file
Open the file `examples/count.in` from within the REPL using `\o`:

#### examples/count.in
```
let c = fn(x, g) {
    if(x < g) {
        return c(x + 1, g);
    } else {
        return x * 2;
    }
};


let fib = fn(x) {
    if(x == 0) {
        return 0;
    } else {
        if(x == 1) {
            return 1;
        } else {
            return fib(x - 1) + fib(x - 2);
        }
    }
};

c(0, 1000);
fib(10);
```
#### Output
```
Type '\h' for help and '\q' to quit
>>> \o
*** Open file: examples/count.in
File "examples/count.in" opened
2000
55
>>> \q
Bye!
```
### Built-in functions
The language supports a few built-in functions: `len`, `find`, `str`, `push`, `first`, `last`, `rest`, and `type`.
#### len(`string` or `array`)
```
>>> len("abcde")
5
>>> len([1, 2, "z"]) + 1
4
```
#### find(`string`, `string`)
```
>>> find("abc", "c")
2
>>> find("abc", "d")
-1
```
#### string(`non-string`)
````
>>> str([1, 2]) + " (string)"
'[1, 2] (string)'
````
#### push(`array`, `object`)
````
>>> let a = [1];
[1]
>>> push(a, 2);
NULL
>>> a
[1, 2]
````
#### first(`array`), last(`array`), rest(`array`)
````
>>> first(["a", "b"])
'a'
>>> last([1, 2])
2
>>> rest([1,2,3])
[2, 3]
````
#### type(`object`)
````
>>> type({'a': 1})
'HASHMAP'
>>> type([1])
'ARRAY'
>>> type(fn(x) { return false; })
'FUNCTION'
````
## Todo
- [ ] Implement mark-and-sweep garbage collection
- [ ] Include support for more control flow statements
- [ ] Add custom type support
- [ ] Finish the compiler implementation in Thorsten Ball's ["Writing A Compiler In Go"](https://compilerbook.com/)
