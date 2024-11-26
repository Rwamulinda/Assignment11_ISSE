/*
 * expr_tree.c
 *
 * A dynamically allocated tree to handle arbitrary arithmetic
 * expressions
 *
 * Author: <Pauline Uwase>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "expr_tree.h"
#include "cdict.h"

#define LEFT 0
#define RIGHT 1

struct _expr_tree_node
{
    ExprNodeType type;
    union
    {
        struct _expr_tree_node *child[2];
        double value;
        char *symbol; // For SYMBOL type
    } n;
};

/*
 * Convert an ExprNodeType into a printable character
 *
 * Parameters:
 *   ent    The ExprNodeType to convert
 *
 * Returns: A single character representing the ent
 */
static char ExprNodeType_to_char(ExprNodeType ent)
{
    switch (ent)
    {
    case OP_ADD:
        return '+';
    case OP_SUB:
        return '-';
    case OP_MUL:
        return '*';
    case OP_DIV:
        return '/';
    case OP_POWER:
        return '^';
    case OP_ASSIGN:
        return '=';
    case UNARY_NEGATE:
        return '-';
    default:
        return '?';
    }
}

// Documented in .h file
ExprTree ET_value(double value)
{
    ExprTree new = (ExprTree)malloc(sizeof(struct _expr_tree_node));
    assert(new);
    new->type = VALUE;
    new->n.value = value;
    return new;
}

// Documented in .h file
ExprTree ET_node(ExprNodeType op, ExprTree left, ExprTree right)
{
    ExprTree new = (ExprTree)malloc(sizeof(struct _expr_tree_node));
    assert(new);
    new->type = op;
    new->n.child[LEFT] = left;
    new->n.child[RIGHT] = right;
    return new;
}
// Add SYMBOL node support
ExprTree ET_symbol(const char *symbol)
{
    ExprTree new = (ExprTree)malloc(sizeof(struct _expr_tree_node));
    assert(new);
    new->type = SYMBOL;
    new->n.symbol = strdup(symbol); // Allocate memory for the symbol
    return new;
}

// Documented in .h file
void ET_free(ExprTree tree)
{
    if (tree == NULL)
        return;

    if (tree->type == VALUE)
    {
        free(tree);
        return;
    }

    if (tree->type == SYMBOL)
    {
        free(tree->n.symbol); // Free symbol string
        free(tree);
        return;
    }

    if (tree->type != VALUE)
    {
        ET_free(tree->n.child[LEFT]);
        ET_free(tree->n.child[RIGHT]);
    }

    free(tree);
}

// Documented in .h file
int ET_count(ExprTree tree)
{
    if (tree == NULL)
        return 0;

    if (tree->type == VALUE || tree->type == SYMBOL)
        return 1;

    return 1 + ET_count(tree->n.child[LEFT]) + ET_count(tree->n.child[RIGHT]);
}

// Documented in .h file
int ET_depth(ExprTree tree)
{
    if (tree == NULL)
        return 0;
    if (tree->type == VALUE || tree->type == SYMBOL)
        return 1;

    int left_depth = ET_depth(tree->n.child[LEFT]);
    int right_depth = ET_depth(tree->n.child[RIGHT]);
    return 1 + (left_depth > right_depth ? left_depth : right_depth);
}

// Documented in .h file
// Updated ET_evaluate
double ET_evaluate(ExprTree tree, CDict vars, char *errmsg, size_t errmsg_sz)
{
    if (tree == NULL)
        return 0;

    if (tree->type == VALUE)
        return tree->n.value;

    if (tree->type == SYMBOL)
    {
        CDictValueType val = CD_retrieve(vars, tree->n.symbol);
        if (isnan(val))
        {
            snprintf(errmsg, errmsg_sz, "Undefined variable: %s", tree->n.symbol);
            return NAN;
        }
        return val;
    }


    // TODO: Before going to the left first handle the OP_ASSIGN type
    if (tree->type == OP_ASSIGN)
    {
        // Directly store the evaluated right-hand value in the left-hand symbol
        double value = ET_evaluate(tree->n.child[RIGHT], vars, errmsg, errmsg_sz);
        CD_store(vars, tree->n.child[LEFT]->n.symbol, value);
        return value; // Return the assigned value
    }

    double left_val = ET_evaluate(tree->n.child[LEFT], vars, errmsg, errmsg_sz);
    double right_val = ET_evaluate(tree->n.child[RIGHT], vars, errmsg, errmsg_sz);

    switch (tree->type)
    {
    case OP_ADD:
        return left_val + right_val;
    case OP_SUB:
        return left_val - right_val;
    case OP_MUL:
        return left_val * right_val;
    case OP_DIV:
        if (right_val == 0)
        {
            snprintf(errmsg, errmsg_sz, "Error: Division by zero");
            return NAN;
        }
        return left_val / right_val;
    case OP_POWER:
        return pow(left_val, right_val);
    case UNARY_NEGATE:
        return -left_val;
    case OP_ASSIGN:
        if (tree->n.child[LEFT]->type != SYMBOL)
        {
            snprintf(errmsg, errmsg_sz, "Error: Left side of '=' must be a variable");
            return NAN;
        }
        CD_store(vars, tree->n.child[LEFT]->n.symbol, right_val);

        return right_val;
    default:
        snprintf(errmsg, errmsg_sz, "Error: Invalid operator");
        return NAN;
    }
}

// Documented in .h file
size_t ET_tree2string(ExprTree tree, char *buf, size_t buf_sz)
{
    if (buf_sz == 0 || tree == NULL)
        return 0;

    size_t len = 0;

    // Handle VALUE nodes
    if (tree->type == VALUE)
    {
        len = snprintf(buf, buf_sz, "%g", tree->n.value);
        return len;
    }

    // Handle SYMBOL nodes
    if (tree->type == SYMBOL)
    {
        len = snprintf(buf, buf_sz, "%s", tree->n.symbol);
        return len;
    }

    // Handle UNARY NEGATE type specifically
    if (tree->type == UNARY_NEGATE)
    {
        if (buf_sz < 2) return 0; // not enough space for '(' and '-'

        buf[len++] = '(';
        if (len < buf_sz - 1)
        {
            buf[len++] = ExprNodeType_to_char(UNARY_NEGATE);
        }

        size_t child_len = ET_tree2string(tree->n.child[LEFT], buf + len, buf_sz - len);
        len += child_len;

        // Check if the buffer overflowed
        if (len >= buf_sz)
        {
            buf[buf_sz - 1] = '$'; // indication of truncation
            buf[buf_sz - 2] = '\0';
            return buf_sz - 1;
        }

        // Add closing parenthesis
        if (len < buf_sz - 1)
        {
            buf[len++] = ')';
        }

        buf[len] = '\0';
        return len;
    }

    // For operators (binary expressions)
    char op = ExprNodeType_to_char(tree->type);

    if (buf_sz < 3) return 0; // need space for at least one operator and parens

    buf[len++] = '(';

    // Handle the left child
    size_t left_len = ET_tree2string(tree->n.child[LEFT], buf + len, buf_sz - len);
    len += left_len;
    if (len >= buf_sz) return len; // prevent buffer overrun

    buf[len++] = op;

    // Handle the right child
    size_t right_len = ET_tree2string(tree->n.child[RIGHT], buf + len, buf_sz - len);
    len += right_len;
    if (len >= buf_sz) return len; // prevent buffer overrun

    buf[len++] = ')';
    buf[len] = '\0';

    return len;
}
