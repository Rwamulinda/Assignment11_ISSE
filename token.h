/*
 * token.h
 * 
 * Enum containing all possible tokens
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 */

#ifndef _TOKEN_H_
#define _TOKEN_H_

typedef enum {
  TOK_VALUE,
  TOK_SYMBOL,
  TOK_EQUAL,
  TOK_PLUS,
  TOK_MINUS,
  TOK_MULTIPLY,
  TOK_DIVIDE,
  TOK_POWER,
  TOK_OPEN_PAREN,
  TOK_CLOSE_PAREN,
  TOK_END
} TokenType;

#define SYMBOL_MAX_SIZE 31

typedef struct {
  TokenType type;
  union {
    double value;
    char symbol[SYMBOL_MAX_SIZE+1];
  } t;
} Token;


#endif /* _TOKEN_H_ */
