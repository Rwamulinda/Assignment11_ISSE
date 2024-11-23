/*
 * expr_whizz.c
 * 
 * A recursive descent parser for a simple arithmetic calculator, with
 * the operators + - * / % ^, and unary minus. Values are held as
 * doubles.
 *
 * Author: <your name here>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "clist.h"
#include "token.h"
#include "tokenize.h"
#include "expr_tree.h"
#include "parse.h"

int main(int argc, char *argv[])
{
  char *input = NULL;
  CList tokens = NULL;
  ExprTree tree = NULL;
  char errmsg[128];
  bool time_to_quit = false;
  char expr_buf[1024];
  CDict dict = CD_new();

  printf("Welcome to ExpressionWhizz!\n");

  while (!time_to_quit) {
    errmsg[0] = '\0';
    
    input = readline("\nExpr? ");
    if (input == NULL || strcasecmp(input, "quit") == 0) {
      time_to_quit = true;
      goto loop_end;
    }

    if (*input == '\0')   // user just hit enter, no content
      goto loop_end;

    add_history(input);

    tokens = TOK_tokenize_input(input, errmsg, sizeof(errmsg));

    if (tokens == NULL) {
      fprintf(stderr, "%s\n", errmsg);
      goto loop_end;
    }

    if (CL_length(tokens) == 0)
      goto loop_end;
      
    // uncomment for more debug info
    TOK_print(tokens);

    tree = Parse(tokens, errmsg, sizeof(errmsg));

    if (tree == NULL) {
      fprintf(stderr, "%s\n", errmsg);
      goto loop_end;
    }

    ET_tree2string(tree, expr_buf, sizeof(expr_buf));
    
    double result = ET_evaluate(tree, dict, errmsg, sizeof(errmsg));
    if (*errmsg != '\0') { // Error handling
      fprintf(stderr, "Error: %s\n", errmsg);
    } else {
      printf("%s  ==> %g\n", expr_buf, result);
    }

  loop_end:
    free(input);
    input = NULL;
    CL_free(tokens);
    tokens = NULL;
    ET_free(tree);
    tree = NULL;
  }
  
  //TODO: Free the dictionary
  return 0;
}
