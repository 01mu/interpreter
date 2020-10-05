/*
 *
 * interpreter
 * github.com/01mu/interpreter
 *
 * test/repl.c
 *
 */

void repl_test() {
    char * t[][2] = {
        {"[0,[0,1]][1][1] = 3", "NULL "},
        {"let a = [1,2,[0]]; a[2][0] = 1; a;",
            "[1, 2, [0]] NULL [1, 2, [1]] "},


        {"let a = [1,2,[[1,2],0]]; a[2][0][1] = 1; a;",
            "[1, 2, [[1, 2], 0]] NULL [1, 2, [[1, 1], 0]] "},
        //{"let z = {\"a\": 1, \"b\": \"z\"}; z[\"a\"]; z[\"b\"]", "1 z "},
        // =====================================================================
        {"let c = fn(x) { if(x < 9) { return c(x + 1); } else { return x; } }; \
            let e = c(0); e + 1;", "9 10 "},
        {"let a = fn() { let b = 3; return b; }; a();", "3 3 "},
        {"let a = fn() { return fn(x, y) { 1; return x + y; }; }; \
            let g = a()(1, 2); g;", "1 3 3 "},
        {"let a = 1; if(true) { let a = 55; } else { }; a;", "1 55 55 "},
        {"let e = 1; let a = fn(z) { if(true) { let f = 3; } }; a(e); e;",
            "1 3 NULL 1 "},
        {"let a = fn() { if(true) { if(false) { } else { return 5; }; } \
            else { }; }; let a = a(); a; a; a;", "5 5 5 5 "},
        {"!!!false == true", "1 "},
        {"a/2;", "Identifier not found: a "},
        {"2/a;", "Identifier not found: a "},
        {"return a; 1;", "Identifier not found: a "},
        // 10 ==================================================================
        {"let z = 1; let a = fn(z) { if(true) { return 33; } }; \
            let z = a(z); z;", "1 33 33 "},
        {"true; false; !true; !false; true == false;", "1 0 0 1 0 "},
        {"let a = 2; let b = a; let a = b + 2; a; 3;", "2 2 4 4 3 "},
        {"let v = fn(x) { return x; }(3); v;", "3 3 "},
        {"let n = false; let a = fn() { n; let b = 1; }; a(); a(); b;", \
            "0 0 1 NULL 0 1 NULL Identifier not found: b "},
        {"let a = fn(x) { return 1; }; let a = 3; a; let a = fn(x) \
            { return 1; }; a(3);", "3 3 1 "},
        {"let a = fn() { 11; return 5; ff; }; let d = a(); d;", "11 5 5 "},
        {"let z = 1; let a = fn() { let z = 3; return z; }; \
            let d = a(); z; d;", "1 3 3 3 3 "},
        {"let z = false; let a = fn(z) { let z = true; }; a(z);", "0 1 NULL " },
        {"let c = fn(x) { let g = fn() { return 5; }; return g() + 1; }; \
            let e = c(0); e * e;", "6 36 "},
        // 20 ==================================================================
        {"let c = fn(x, r) { if(x < r) { return c(x + 1, r); } else \
            { return x; } }; let g = fn() { return 12; }; let e = c(0, g()); \
            e * 2;", "12 24 "},
        {"let c = fn(x, g) { if(x < g) { return c(x + 1, g); } else \
            { return x + 1; } }; let g = 20; let e = fn() { c(0, g); }; e(); \
            g;", "20 21 NULL 20 "},
        {"\"asd\"; let z = \"ddd\";", "asd ddd "},
        {"let c = fn(x) { if(x < 9) { return c(x + 1); } else \
            { return \"a\"; } }; let e = c(0); e;", "a a "},
        {"let a = 1; let a = \"z\"; a;", "1 z z "},
        {"let a = \"z\"; let a = \"d\"; a;", "z d d "},
        {"let z = fn(a) { return a; }; let a = z(\"d\");", "d "},
        {"let z = fn(a) { return a; }; z(\"d\");", "d "},
        {"let g = \"z\"; let z = fn(a) { return a; }; z(g);", "z z "},
        {"let z = fn() { let g = \"zzz\"; return g; }; let d = z(); \
            let b = z();", "zzz zzz zzz zzz "},
        // 30 ==================================================================
        {"let a = \"a\"; let b = a; b;", "a a a " },
        {"let f = \"abc\" + \"def\"; f;", "abcdef abcdef "},
        {"\"abc\" + \"def\";", "abcdef "},
        {"1 + 2;", "3 "},
        {"\"asd\" - \"a\"", "Unknown operator: STRING - STRING "},
        {"let z = \"a\"; let d = z + \"b\" + \"c\" + \"a\" + \"b\" + \"c\"; d;",
            "a abcabc abcabc "},
        {"let meme = \"aaaa\"; len(1);",
            "aaaa Argument not a string or an array "},
        {"len(\"z\");", "1 "},
        {"let meme = \"aaaa\"; len(meme + \"z\"); meme;", "aaaa 5 aaaa "},
        {"len(\"aaa\") + len(\"z\");", "4 "},
        // 40 ==================================================================
        {"find(\"a\", \"e\") find(\"abcde\", \"cd\"); \
            find(\"abcde\", \"e\");", "-1 2 4 "},
        {"a + \"zz\" + c + 2; \"zz\" + c + 2 + a;", \
            "Identifier not found: a "},
        {"z; 3; ", "Identifier not found: z "},
        {"let a = 5; let a = a; a;", "5 5 5 "},
        {"\"a\" + str(1) + 1; \"a\" + str(1); str(1) + \"a\";",
                "Type mismatch: STRING + INT "},
        {"let a = if(true) { } else { }; a;", "NULL NULL "},
        {"let a = [fn() { return 1; }, 1, if(true) { } else { }]; \
            let d = a[2];", "[(fn), 1, NULL] NULL "},
        {"[2] != [1,2]; [1,2,3] == [1,2,3,4]; [1,\"a\",[1]] == [1,\"a\",[1]];",
            "1 0 1 "},
        {"let a = \"a\"; a == \"a\";", "a 1 "},
        {"\"a\" == \"a\"; \"a\" == \"b\"; \"a\" != \"a\"" , "1 0 0 "},
        // 50 ==================================================================
        {"let a = fn() { let d = [1]; return [\"a\", \"b\"]; }; \
            let d=a(); d[0];", "[1] [a, b] a "},
        {"let a = fn() { let d = [1]; return d; }; a()[0]; \
            let a = fn() { let d = [1]; return [\"a\", \"b\"]; }; a()[0];",
            "[1] 1 [1] a "},
        {"[1, 2, [1, [2]]][2][1][0]", "2 "},
        {"[1, 2, [1]][2][0]", "1 "},
        {"let e = [1, 2, [\"a\"]][2]; let z = e[0]; z; [[1]];",
            "[a] a a [[1]] "},
        {"[1, 2, [1]][2];", "[1] "},
        {"[1, 2][1];", "2 "},
        {"let a = [1, 2, 3]; let j = 0; a[j]; j;", "[1, 2, 3] 0 1 0 "},
        {"let a = [1, 2, 3]; let d = a[0]; d + 4;", "[1, 2, 3] 1 5 "},
        {"let g = [-5]; let a = fn(g) { let d = [1]; return [d, [2], g]; }; \
            a(g)[0][0] + a(g)[1][0]; a(g)[2][0];", "[-5] [1] [1] 3 [1] -5 "},
        // 60 ==================================================================
        {"let a = [1, 2]; a[0] = 3; a[0]; [0][0] = 2;",
            "[1, 2] NULL 3 Unknown operator: INT = INT "},
    };

    int e = sizeof(t) / sizeof(t[0]);
    //e = 1;
    int b = e, i;
    bool res = false;

    for(i = 0; i < b; i++) {
        res = false;
        repl(1, &res, t[i], &i);

        if(!res) {
            --e;
        }
    }

    if(e == b) {
        printf(ANSI_COLOR_GREEN "\n[PASSED] " ANSI_COLOR_RESET
            "REPL Test | %i out of %i passed\n\n", e, b);
    } else {
        printf(ANSI_COLOR_RED "\n[FAILED] " ANSI_COLOR_RESET
            "REPL Test | %i out of %i passed\n\n", e, b);
    }
}
