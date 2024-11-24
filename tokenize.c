/*
 * tokenize.c
 *
 * Functions to tokenize and manipulate lists of tokens
 *
 * Author: <Uwase Pauline>
 */

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "clist.h"
#include "tokenize.h"
#include "token.h"
#include <stddef.h>

// Documented in .h file
const char *TT_to_str(TokenType tt)
{
  switch (tt)
  {
  case TOK_VALUE:
    return "VALUE";
  case TOK_PLUS:
    return "PLUS";
  case TOK_MINUS:
    return "MINUS";
  case TOK_MULTIPLY:
    return "MULTIPLY";
  case TOK_DIVIDE:
    return "DIVIDE";
  case TOK_POWER:
    return "POWER";
  case TOK_OPEN_PAREN:
    return "OPEN_PAREN";
  case TOK_CLOSE_PAREN:
    return "CLOSE_PAREN";

  case TOK_SYMBOL:
    return "SYMBOL"; // New case for symbols
  case TOK_EQUAL:
    return "EQUAL"; // New case for '=' operator
  case TOK_END:
    return "(end)";
  }

  __builtin_unreachable();
}

// Documented in .h file
CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
  CList tokens = CL_new();
  size_t i = 0;

  while (input[i] != '\0')
  {
    if (isspace(input[i]))
    {
      i++; // Skip whitespace
      continue;
    }

    Token token;

    // Handle numbers (integers and doubles)
    if (isdigit(input[i]) || input[i] == '.')
    {
      char *endptr;
      double value = strtod(&input[i], &endptr);

      if (endptr == &input[i])
      {
        snprintf(errmsg, errmsg_sz, "Position %zu: Illegal numeric value", i + 1);
        CL_free(tokens);
        return NULL;
      }

      // Check if we've consumed any characters for the number
      i += (endptr - &input[i]);
      token.type = TOK_VALUE;
      token.t.value = value;
    }
    else if ((isalpha(input[i]) || input[i] == '_') && i < 31)
    {
      // Handle symbols (variable names)
      size_t start = i;
      size_t length = 0;
      while ((isalnum(input[i]) || input[i] == '_') && length < 31)
      {
        i++;
        length++;
      }

      if (length > 31)
      {
        snprintf(errmsg, errmsg_sz, "Position %zu: Symbol length exceeds maximum of 31 characters", start + 1);
        CL_free(tokens);
        return NULL;
      }

      // Create a symbol token
      token.type = TOK_SYMBOL;
      strncpy(token.t.symbol, &input[start], length);
      token.t.symbol[length] = '\0'; // Null-terminate the symbol
    }
    else if (input[i] == '=')
    {
      // Handle assignment operator '='
      token.type = TOK_EQUAL;
      token.t.value = 0; // '=' does not have a numerical value
      i++;
    }
    else
    {
      // Handle operators and parentheses
      switch (input[i])
      {
      case '+':
        token.type = TOK_PLUS;
        break;
      case '-':
        token.type = TOK_MINUS;
        break;
      case '*':
        token.type = TOK_MULTIPLY;
        break;
      case '/':
        token.type = TOK_DIVIDE;
        break;
      case '^':
        token.type = TOK_POWER;
        break;
      case '(':
        token.type = TOK_OPEN_PAREN;
        break;
      case ')':
        token.type = TOK_CLOSE_PAREN;
        break;
      // case '=':
      //   token.type = TOK_EQUAL;
      //   break;
      default:
        // Handle unexpected characters with specific position
        snprintf(errmsg, errmsg_sz, "Position %zu: unexpected character %c", i + 1, input[i]);
        CL_free(tokens);
        return NULL;
      }
      token.t.value = 0; // Operators and parentheses don't have a value
      i++;
    }

    // Add token to the list
    CL_append(tokens, token);
  }

  // Add end-of-input token
  Token end_token = {.type = TOK_END, .t.value = 0};
  CL_append(tokens, end_token);

  return tokens;
}

// Documented in .h file
TokenType TOK_next_type(CList tokens)
{
  if (CL_length(tokens) == 0)
  {
    return TOK_END;
  }
  Token token = CL_nth(tokens, 0);
  return token.type;
}

// Documented in .h file
Token TOK_next(CList tokens)
{
  return CL_nth(tokens, 0);
}

Token TOK_next_Assignement(CList tokens)
{
  return CL_nth(tokens, 1);
}


// Documented in .h file
void TOK_consume(CList tokens)
{
  if (CL_length(tokens) > 0)
  {
    CL_pop(tokens);
  }
}

void printToken(int pos, CListElementType element, void *cb_data)
{
  if (element.type == TOK_VALUE)
  {
    // print token type and value
    printf("Position %d: Token type: %s, Value: %g\n", pos, TT_to_str(element.type), element.t.value);
  }

  else if (element.type == TOK_SYMBOL) {
    // print token type and symbol
    printf("Position %d: Token type: %s, Symbol: %s\n", pos, TT_to_str(element.type), element.t.symbol);
  }
  else
  {
    // print token type
    printf("Position %d: Token type: %s\n", pos, TT_to_str(element.type));
  }
}

// Documented in .h file
void TOK_print(CList tokens)
{
  CL_foreach(tokens, printToken, NULL);
}
