/*
 * parse.h
 * 
 * Simple description
 *
 * Author: <Uwase Pauline>
 */


#ifndef _PARSE_H_
#define _PARSE_H_

#include "clist.h"
#include "expr_tree.h"

/*
 * Parses a list of tokens into an ExprTree, which is the abstract
 * syntax tree for the ExpressionWhizz grammar.  See the assignment
 * writeup for the BNF.
 *
 * Parameters:
 *   tokens     List of tokens remaining to be parsed
 *   errmsg     Return space for an error message, filled in in case of error
 *   errmsg_sz  The size of errmsg
 * 
 * Returns: The parsed ExprTree on success. If a parsing error is
 *   encountered, copies an error message into errmsg and returns
 *   NULL.
 */
ExprTree Parse(CList tokens, char *errmsg, size_t errmsg_sz);

#endif /* _PARSE_H_ */
