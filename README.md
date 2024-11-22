# Assignment9_ISSE
This project is an  interactive assignement  expression evaluator, capable of tokenizing, parsing, and evaluating mathematical expressions with support for basic arithmetic operations, exponentiation, parentheses, and error handling. It utilizes a recursive descent parser, creating a syntax tree to evaluate expressions accurately based on operator precedence and associativity. and we implemented diffrent function in tokenize as well as in purse. we use previous assignment lik clist and expr_tree assignment to accomplish both tokenize and expree tree.

Features
Basic Operations: Supports addition (+), subtraction (-), multiplication (*), and division (/).
Unary Operations: Handles unary operations such as negative numbers (e.g., -5).
Exponentiation: Supports exponentiation with the ^ operator (e.g., 2 ^ 3).
Scientific Notation: Parses numbers in scientific notation (e.g., 3e10).
Error Handling: Provides detailed error messages for invalid expressions, such as unexpected tokens, unmatched parentheses, and illegal numeric values.
Syntax Tree Representation: Constructs a syntax tree for accurate expression parsing and evaluation.
Files
tokenize.c: Contains the tokenizer, responsible for breaking input strings into tokens that represent numbers, operators, and parentheses.
expr_tree.c: Implements the expression tree data structure, storing parsed expressions and evaluating them.
parser.c: Contains the recursive descent parser that processes tokens and builds the expression tree.
ew_test.c: Provides unit tests for tokenization, parsing, and evaluation.
clist.c and clist.h: Implements a circular linked list used for managing token streams.
README.md: Project documentation.