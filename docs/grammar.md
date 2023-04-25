# YScript Grammar

## YScript Lexical Grammar

```
NUMBER ::= DIGIT+ ( "." DIGIT+ )? ;

STRING ::= "\"" <any char except "\"">* "\"" ;

IDENTIFIER ::= ALPHA ( ALPHA | DIGIT )* ;

ALPHA ::= "a" ... "z" | "A" ... "Z" | "_" ;

DIGIT ::= "0" ... "9" ;
```

## YScript BNF Syntax Grammar

```
Module ::= Decl* EOF ; # translate unit

;(* Declarations *)

Decl ::= classDecl | funDecl | varDecl | statement ;

classDecl ::= "class" IDENTIFIER (":" IDENTIFIER (, IDENTIFIER)*)? "{"
  function* "
}" ;

funDecl ::= "fun" function ;

varDecl ::= "var" IDENTIFIER ( "=" expression )? ";" ;

;(* Statements *)

statement ::= exprStmt | forStmt | ifStmt | printStmt
              | returnStmt | whileStmt | block ;

exprStmt ::= expression ";" ;

forStmt ::= "for" "(" ( varDecl | exprStmt | ";" )
                      expression? ";"
                      expression? ")" statement ;

ifStmt ::= "if" "(" expression ")" statement
           ( "else" statement )? ;

printStmt ::= "print" expression ";" ;

returnStmt ::= "return" expression? ";" ;

whileStmt ::= "while" "(" expression ")" statement ;

block ::= "{" declaration* "}" ;

;(* Expressions *)

expression ::= assignment ;

assignment ::= ( call "." )? IDENTIFIER "=" assignment | logic_or ;

logic_or ::= logic_and ( "or" logic_and )* ;

logic_and ::= equality ( "and" equality )* ;

equality ::= comparison ( ( "!=" | "==" ) comparison )* ;

comparison ::= term ( ( ">" | ">=" | "<" | "<=" ) term )* ;

term ::= factor ( ( "-" | "+" ) factor )* ;

factor ::= unary ( ( "/" | "*" ) unary )* ;

unary ::= ( "!" | "-" ) unary | call ;

call ::= primary ( "(" arguments? ")" | "." IDENTIFIER )* ;

primary ::= "true" | "false" | "nil" | "this"
            | NUMBER | STRING | IDENTIFIER | "(" expression ")"
            | "super" "." IDENTIFIER ;


function ::= IDENTIFIER "(" parameters? ")" block ;

parameters ::= IDENTIFIER ( "," IDENTIFIER )* ;

arguments ::= expression ( "," expression )* ;

```

## Sample
```
class Foo {
  inFoo() {
    print "in foo";
  }
}

class Bar : Foo {
  inBar() {
    print "in bar";
  }
}

class Baz : Bar {
  inBaz() {
    print "in baz";
  }
}

var baz = Baz();
baz.inFoo(); // expect: in foo
baz.inBar(); // expect: in bar
baz.inBaz(); // expect: in baz

```

compile and run with [ysrun](../tools/cli/README.md)

```
== inFoo ==
0000    3 OP_CONSTANT         0 'in foo'
0002    | OP_PRINT
0003    4 OP_NIL
0004    | OP_RETURN
== inBar ==
0000    9 OP_CONSTANT         0 'in bar'
0002    | OP_PRINT
0003   10 OP_NIL
0004    | OP_RETURN
== inBaz ==
0000   15 OP_CONSTANT         0 'in baz'
0002    | OP_PRINT
0003   16 OP_NIL
0004    | OP_RETURN
== <script> ==
0000    1 OP_CLASS            0 'Foo'
0002    | OP_DEFINE_GLOBAL    0 'Foo'
0004    | OP_GET_GLOBAL       1 'Foo'
0006    4 OP_CLOSURE          3 <fn inFoo>
0008    | OP_METHOD           2 'inFoo'
0010    5 OP_POP
0011    7 OP_CLASS            4 'Bar'
0013    | OP_DEFINE_GLOBAL    4 'Bar'
0015    | OP_GET_GLOBAL       5 'Foo'
0017    | OP_GET_GLOBAL       6 'Bar'
0019    | OP_INHERIT
0020    | OP_GET_GLOBAL       7 'Bar'
0022   10 OP_CLOSURE          9 <fn inBar>
0024    | OP_METHOD           8 'inBar'
0026   11 OP_POP
0027    | OP_POP
0028   13 OP_CLASS           10 'Baz'
0030    | OP_DEFINE_GLOBAL   10 'Baz'
0032    | OP_GET_GLOBAL      11 'Bar'
0034    | OP_GET_GLOBAL      12 'Baz'
0036    | OP_INHERIT
0037    | OP_GET_GLOBAL      13 'Baz'
0039   16 OP_CLOSURE         15 <fn inBaz>
0041    | OP_METHOD          14 'inBaz'
0043   17 OP_POP
0044    | OP_POP
0045   19 OP_GET_GLOBAL      17 'Baz'
0047    | OP_CALL             0
0049    | OP_DEFINE_GLOBAL   16 'baz'
0051   20 OP_GET_GLOBAL      18 'baz'
0053    | OP_INVOKE        (0 args)   19 'inFoo'
0056    | OP_POP
0057   21 OP_GET_GLOBAL      20 'baz'
0059    | OP_INVOKE        (0 args)   21 'inBar'
0062    | OP_POP
0063   22 OP_GET_GLOBAL      22 'baz'
0065    | OP_INVOKE        (0 args)   23 'inBaz'
0068    | OP_POP
0069   23 OP_NIL
0070    | OP_RETURN
in foo
in bar
in baz

```