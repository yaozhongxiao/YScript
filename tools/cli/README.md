
# ysrun

cli used to execute the yscript programs.

```
[~/Workspace/Dev/yscript] ./build.sh --force

[~/Workspace/Dev/yscript]$ out/tools/cli/ysrun testing/samples/function/print.ys 
  == foo ==
  0000    1 OP_NIL
  0001    | OP_RETURN
  == <script> ==
  0000    1 OP_CLOSURE          1 <fn foo>
  0002    | OP_DEFINE_GLOBAL    0 'foo'
  0004    2 OP_GET_GLOBAL       2 'foo'
  0006    | OP_PRINT
  0007    4 OP_GET_GLOBAL       3 'clock'
  0009    | OP_PRINT
  0010    5 OP_NIL
  0011    | OP_RETURN
            [ <script> ]
  0000    1 OP_CLOSURE          1 <fn foo>
            [ <script> ][ <fn foo> ]
  0002    | OP_DEFINE_GLOBAL    0 'foo'
            [ <script> ]
  0004    2 OP_GET_GLOBAL       2 'foo'
            [ <script> ][ <fn foo> ]
  0006    | OP_PRINT
  <fn foo>
            [ <script> ]
  0007    4 OP_GET_GLOBAL       3 'clock'
            [ <script> ][ <native fn> ]
  0009    | OP_PRINT
  <native fn>
            [ <script> ]
  0010    5 OP_NIL
            [ <script> ][ nil ]
  0011    | OP_RETURN

```