#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <algorithm>

using namespace std;

int getOperatorPriority(char& c);
void showlist(list <string> g);
string trimInfixExpr(string &str);
list <string> convertInfixToPostfix(string infixExpr);
void convertPostfixToAssembly(list <string> postfixExpr);
void locateCurrentPostfixParameter(list <string> &postfixExpr, string &result);

/** @brief Main function that receives and infix expression 
 *  that will show the result of postfix expression in console.
 * 
 *  Generates assembly instructions and locate them in an asm
 *  file that will be compiled with an 8086 compliant compiler
 *  and this program will write the result of expression in decimal
 *
 *  @param postfixExpr String list that includes postfix elements.
 *  @return void
 */
int main(int argc,  char *argv[]) {

	list <string> postfixExpression;

	if(argc==1)
	{
        printf("\nNo Extra Command Line Argument Passed Other Than Program Name");
	}
    else if(argc>=2)
    {	
		string infixExpression(argv[1]); //put infix commanline argument array into a string
    	cout << "Infix expression is: " << infixExpression << "\n" << endl;

		//Convert infix expression to postfix
		postfixExpression = convertInfixToPostfix(infixExpression);

		//Convert postfix expression to assembly
		convertPostfixToAssembly(postfixExpression);
	}
	else
	{}

	return 0;
}

/** @brief Convert postfix expression to a 8086 assembly program
 *  that will show the result of postfix expression in console.
 * 
 *  Generates assembly instructions and locate them in an asm
 *  file that will be compiled with an 8086 compliant compiler
 *  and this program will write the result of expression in decimal
 *
 *  @param postfixExpr String list that includes postfix elements.
 *  @return void
 */
void convertPostfixToAssembly(list <string> postfixExpr) {

	stack<string> st; //For stack operations, we are using C++ built in stack
	string result;

	//Create and open an .asm file that will store the generated assembly instructions
	ofstream assembly ("assembly.asm", std::ofstream::out);

	//Locate code start
	assembly << "code segment" << "\n\n";

	//Locate string variable that will include string form of result
	assembly << "str db 6 dup('$') ;STRING TO STORE NUMBER." << "\n"; 


	for (string &s : postfixExpr)
	{
		if(s.empty()  || (s == " ") || (s == "\t")) //if character is space or escape character, skip it
			continue;

		// If the scanned operand is an not an operator, push it to stack.
		if((s != "+") && (s != "*") && (s != "/"))
		{
			st.push(s);
		}
		else 
		{
			int i=0;
			//Get first two operands from stack, then look at the oprator between them and 
			//calculate assembly instruction for executing the operator with these operands
			while( !st.empty() && i<2 ) 
			{
				string op1 = st.top();
				if(s == "+")
				{
					result.clear();
					if(op1.size() > 1)
					{
						result += "MOV BX,0x"; 
						result += op1;
						result += "\n";
						result += "PUSH BX";
					}else
					{
						result += "PUSH 0x";
						result += op1;
					}
						
					assembly << result << "\n";
					st.pop();
				}
				else if(s == "*")
				{
					result.clear();
					if(op1.size() > 1)
					{
						result += "MOV BX,0x";
						result += op1;
						result += "\n";
						result += "PUSH BX";
					}else
					{
						result += "PUSH 0x";
						result += op1;
					}
					
					assembly << result << "\n";
					st.pop();
				}
				else if(s == "/")
				{
					result.clear();
					if(op1.size() > 1)
					{
						result += "MOV BX,0x";
						result += op1;
						result += "\n";
						result += "PUSH BX";
					}else
					{
						result += "PUSH 0x";
						result += op1;
					}

					assembly << result << "\n";
					st.pop();
				}
				else{
				}

				i++;
			}

			//If scanned postfix element is and operator, then locate correspoding instruction for
			// executing the opeator by using the operands
			if(s == "+")
			{ 
				result = "POP CX";
				assembly << result << "\n";

				result = "POP AX";
				assembly << result << "\n";

				result = "ADD AX,CX";
				assembly << result << "\n";

				result = "PUSH AX";
				assembly << result << "\n";
			}else if(s == "*")
			{
			
				result = "POP CX";
				assembly << result << "\n";

				result = "POP AX";
				assembly << result << "\n";

				result = "MUL CX";
				assembly << result << "\n";

				result = "PUSH AX";
				assembly << result << "\n"; 
			}else if(s == "/")
			{
				result = "MOV DX,0";
				assembly << result << "\n";

				result = "POP CX";
				assembly << result << "\n";

				result = "POP AX";
				assembly << result << "\n";

				result = "DIV CX";
				assembly << result << "\n";

				result = "PUSH AX";
				assembly << result << "\n"; 
			}
			else{}
		}
	}

	//Locate function code that displays converted string value form of AX register
	string displayOutput= "\n\ncall number2string  ;CONVERT AX. RESULT IN STR.\n"
	";DISPLAY STRING.\n"
  	"mov  ah, 9\n"
  	"mov  dx, offset str  ;NUMBER CONVERTED.\n"
  	"int  21h\n\n";

	assembly << displayOutput << "\n";

	//Locate code section that will end up the program and return to OS
	assembly << "int 20h ; return to OS" << "\n"; 
	assembly << "code ends" << "\n"; 

	//Locate function code that converts hexadecimal value of AX register to string in generated assembly file
	string numberToStringConverterFunctionCode=
	"\n\n;converts number in AX, ectracts digits one by one, push them in stack, then pop them to construct string\n"
	"proc number2string \n"
	"mov  bx, 10 ;DIGITS ARE EXTRACTED DIVIDING BY 10.\n" 
	"mov  cx, 0 ;COUNTER FOR EXTRACTED DIGITS.\n" 
	"cycle1:\n"       
	"mov  dx, 0 ;NECESSARY TO DIVIDE BY BX.\n"  
	"div  bx ;DX:AX / 10 = AX:QUOTIENT DX:REMAINDER.\n"     
	"push dx ;PRESERVE DIGIT EXTRACTED FOR LATER.\n" 	
	"inc  cx ;INCREASE COUNTER FOR EVERY DIGIT EXTRACTED.\n" 	
	"cmp  ax, 0 ;IF NUMBER IS\n"  
	"jne  cycle1 ;NOT ZERO, LOOP.\n"  
	"mov  si, offset str ;NOW RETRIEVE PUSHED DIGITS.\n"	
	"cycle2:\n"  
	"pop  dx\n"       
	"add  dl, 48 ;CONVERT DIGIT TO CHARACTER.\n"  
	"mov  [ si ], dl\n" 
	"inc  si\n"   
	"loop cycle2\n"   
	"ret\n"  
	"endp\n";

	assembly << numberToStringConverterFunctionCode << "\n";
	
	assembly.close();

	cout << "Assembly code generation is completed successfully. Program is stored in assembly.asm in the same folder\n\n" << endl;
}


/** @brief convert infix expression to postfix expression
 *
 *  Gets infix expression as a string, converts it to postfix 
 *  and locates it in a string List
 *
 *  @param infixExpr The new row for the cursor.
 *  @return String list that includes elements of postfix expression.
 */
list <string> convertInfixToPostfix(string infixExpr) {

	stack<char> st; //For stack operations, we are using C++ built in stack
	string result;
	list <string> postfixExpression; //We will keep members of postfix in a string list 

	//Remove space(' ') and escape(\t) characters from infix expression
	infixExpr = trimInfixExpr(infixExpr);

	cout << "Infix Expression is trimmed:" << infixExpr << "\n" << endl;

	//Scan infix characters and evaluate them
	for(int i = 0; i < infixExpr.length(); i++) {

		char c = infixExpr[i];

		// If the scanned character is an operand, add it to output string.
		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
			result += c;

		// If the scanned character is an ‘(‘, push it to the stack.
		else if(c == '(')
			st.push('(');

		// If the scanned character is an ‘)’,pop and to output string from the stack until an ‘(‘ is encountered.
		else if(c == ')') {

			locateCurrentPostfixParameter(postfixExpression, result);
			
			while(!st.empty() && st.top() != '(')
			{
				char temp = st.top();
				st.pop();
				result += temp;

				locateCurrentPostfixParameter(postfixExpression, result);
			}
			if(!st.empty())
				st.pop();
		}

		//If an operator is scanned
		else {
	
			locateCurrentPostfixParameter(postfixExpression, result);

			//Get operator from stack and locate it according to priority of it
			while(!st.empty() && getOperatorPriority(infixExpr[i]) <= getOperatorPriority(st.top())) {
				char temp = st.top();
				st.pop();
				result += temp;
		
				locateCurrentPostfixParameter(postfixExpression, result);
			}
			st.push(c);
		}

	}
	
	locateCurrentPostfixParameter(postfixExpression, result);

	// Pop all the remaining elements from the stack
	while(!st.empty()) {
		char temp = st.top();
		st.pop();
		result = temp;
		locateCurrentPostfixParameter(postfixExpression, result);
	}

	showlist(postfixExpression);

	return postfixExpression;
}


/** @brief put current postfix element to stack for further usage
 *
 *  Locates current postfix element into string list that will 
 *  include total list of postfix elements
 *
 *  @param postfixExpr The new row for the cursor.
 *  @param result Current postfix element
 *  @return void
 */
void locateCurrentPostfixParameter(list <string> &postfixExpr, string &result)
{
	if(!result.empty())
	{
		postfixExpr.push_back(result);
		result="";
	}
}

/** @brief Trimmes the input string by removing spaces and 
 *  escape characters from it.
 * 
 *
 *  @param str String value that wil be trimmed.
 *  @return Returns trimmed form of input string
 */
string trimInfixExpr(string &str) 
{
   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
   return str;
}


/** @brief Prints elements of a list
 * 
 *
 *  @param g String list that will be printed
 *  @return void
 */
void showlist(list <string> g)
{
    list <string> :: iterator it;
	cout << "Calculated PostFix Expression is:" << "\n" << endl;
    for(it = g.begin(); it != g.end(); ++it)
        cout << *it << ' ';
    cout << "\n\n";
}


/** @brief Returns the priority value of the operator
 * 
 *  @param c Character value that includes operator 
 *  that could be +, * and /
 *  @return Returns priority of the operator
 */
int getOperatorPriority(char& c) {
	int pri;

	if(c == '*' || c == '/')
		pri = 2;
	else if(c == '+' || c == '-')
		pri = 1;
	else
		pri = -1;
	
	return pri;
}
