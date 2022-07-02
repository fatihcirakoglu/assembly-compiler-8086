# ASSEMBLY COMPILER WITH C++ 
## SWE 514 Computer Systems Spring 2021 Project - Fatih Cirakoglu

In this project, our aim is to develop a basic assembly compiler that will process given infix expression and will generate an
assembly program that calculates and prints the result of infix expression on console.

This program's name will be exprassemble and will receive infix expression from commandline, then will generate assembly code for this
infix expression. Also it will handle spaces and escape characters in infix expression.

Program accepts one parameter from commandline. But if you run this program in Linux environment, () are used in bash to create a sub-shell in Linux. So either you need to escape them as \( and \) to pass them as command line arguments or you should git infix parameter by putting it in "".

After getting infix expression from commandline, firstly I give it a fucntion that is named as convertInfixToPostfix which fets infix expression as a string, converts it to postfix and then locates it in a string List. In this function it passes infix expression to a function that removes the escape and space characters from infix expression.

After removing the space and escape characters, this function starts scanning the expression in a for loop until last character in the infix 
expression. While scanning infix expression, if the scanned character is an operand, it will add it to output string.
If the scanned character is an ‘(‘, push it to the stack. If the scanned character is an ‘)’,pop stack and pass value to output string from the stack until an ‘(‘ is encountered. By this way, we will pass all variables between two bracksts () except the operator and locate them in postfix list.
When an operator is scanned, we handle it sepetrately. While locating the operator in postfix expression, we get the operator type and get its
priority since * and / operators have higher priority then + operator.
At the end of convertInfixToPostfix function, a list is completed and includes complete postfix expression, it will be written on console.

After creating the postfix expression, I pass it to convertPostfixToAssembly function for generation of assembly code. This function goes through the 
elements of list that includes postfix expression elements. It also checks whether there are space and escape characters. 
This function pushes all elements except operators to stack firstly. If first operator is detected, then the two variables that are pushed to stack 
will be used. I pop these variables from stack and locate them in general purpose registers. If one of the variable is larger then 1 byte, then I 
push this variable via register to prevent default stack assignment in leading bits. For example when I push 0xAB directly to stack, it keeps it as 
0xFFAB and then I pop this value as is and this causes problem. 

If scanned postfix element is and operator, then locate correspoding instruction for executing the opeator by using the two operands. 

I execute this operation in a loop and by this way, I generate corresponding assembly code for each postfix element that is accepted as operand. 
I also locate the additional code for writing the result of infix expression on console. Assembly code is being generated for 8086 cpu family. 

This program doesnt support 16 bit multiplication, so every calculated midterm and final values will fit in 16 bit. 

## Build:
For compiling program in Linux and Windows environment, you can use below command. 

   g++ exprassemble.cpp -o exprassemble

## Usage: 
 Run like below: 

 $ ./exprassemble "(((ab + ff) * 2) / (ad + cff) * (3 + (5*a)))"

 Output will be like below: 
- Infix expression is: (((ab + ff) * 2) / (ad + cff) * (3 + (5*a)))

- Infix Expression is trimmed:(((ab+ff)*2)/(ad+cff)*(3+(5*a)))

- Calculated PostFix Expression is:

    ab ff + 2 * ad cff + / 3 5 a * + * 

Assembly code generation is completed successfully. Program is stored in assembly.asm in the same folder
- assembly.asm:
```
code segment

str db 6 dup('$') ;STRING TO STORE NUMBER.
MOV BX,0xff
PUSH BX
MOV BX,0xab
PUSH BX
POP CX
POP AX
ADD AX,CX
PUSH AX
PUSH 0x2
POP CX
POP AX
MUL CX
PUSH AX
MOV BX,0xcff
PUSH BX
MOV BX,0xad
PUSH BX
POP CX
POP AX
ADD AX,CX
PUSH AX
MOV DX,0
POP CX
POP AX
DIV CX
PUSH AX
PUSH 0xa
PUSH 0x5
POP CX
POP AX
MUL CX
PUSH AX
PUSH 0x3
POP CX
POP AX
ADD AX,CX
PUSH AX
POP CX
POP AX
MUL CX
PUSH AX


call number2string  ;CONVERT AX. RESULT IN STR.
;DISPLAY STRING.
mov  ah, 9
mov  dx, offset str  ;NUMBER CONVERTED.
int  21h


int 20h ; return to OS
code ends


;converts number in AX, ectracts digits one by one, push them in stack, then pop them to construct string
proc number2string 
mov  bx, 10 ;DIGITS ARE EXTRACTED DIVIDING BY 10.
mov  cx, 0 ;COUNTER FOR EXTRACTED DIGITS.
cycle1:
mov  dx, 0 ;NECESSARY TO DIVIDE BY BX.
div  bx ;DX:AX / 10 = AX:QUOTIENT DX:REMAINDER.
push dx ;PRESERVE DIGIT EXTRACTED FOR LATER.
inc  cx ;INCREASE COUNTER FOR EVERY DIGIT EXTRACTED.
cmp  ax, 0 ;IF NUMBER IS
jne  cycle1 ;NOT ZERO, LOOP.
mov  si, offset str ;NOW RETRIEVE PUSHED DIGITS.
cycle2:
pop  dx
add  dl, 48 ;CONVERT DIGIT TO CHARACTER.
mov  [ si ], dl
inc  si
loop cycle2
ret
endp

```

So take assembly.asm and compile/run it in a 8086 assembly compiler. 

Below tests are done for confirming the result of application.  
- ### Test1: ( ( ff + ( 3 *2 ) ) / ( 1 + 2 ) ) ) = 87(57h) (PASSED)
- ### Test2: (ab43+2)/3 + (ab *3) + 5 = 3B1Dh (PASSED)
- ### Test3: ( ((ab + ff) * 2) / (ad +  cff) * (3 + (5 * a)) ) = 0 (PASSED)