/*
 * cdict.h
 * 
 * Dictionary based on a hash table utilizing open addressing to
 * resolve collisions.
 *
 * Author: <Pauline Uwase>
 */
#ifndef _CDICT_H_
#define _CDICT_H_

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

typedef struct _dictionary *CDict;

typedef const char *CDictKeyType;
typedef double CDictValueType;

#define INVALID_VALUE NAN


/*
 * Returns a newly-allocated and newly-initialized dictionary. Upon
 * return, the dictionary will have the default capacity and no
 * elements.
 *
 * Parameters: None
 * 
 * Returns: The new CDict
 */
CDict CD_new();


/*
 * Destroy all memory consumed by this dict
 *
 * Parameters:
 *   dict     The dictionary
 * 
 * Returns: None
 */
void CD_free(CDict dict);


/*
 * Returns the number of elements in the dictionary; note this is
 * different from its capacity
 *
 * Parameters:
 *   dict     The dictionary
 * 
 * Returns: the dictionary's size
 */
unsigned int CD_size(CDict dict);


/*
 * Returns the capacity of the dictionary, which is the theoretical
 * number of total elements that the dictionary could store before
 * growing, if load factor were 1.0
 *
 * Parameters:
 *   dict     The dictionary
 * 
 * Returns: the dictionary's capacity
 */
unsigned int CD_capacity(CDict dict);


/*
 * Is key found in dictionary?
 *
 * Parameters:
 *   dict     The dictionary
 *   key      The key
 * 
 * Returns: True if key is in dict, false otherwise
 */
bool CD_contains(CDict dict, CDictKeyType key);


/*
 * Store the supplied key, value pair in the dictionary. If key is
 * already present, its value is overwritten.
 *
 * Neither key nor value can be NULL.
 *
 * Parameters:
 *   dict     The dictionary
 *   key      The key
 *   value    The value
 * 
 * Returns: None
 */
void CD_store(CDict dict, CDictKeyType key, CDictValueType value);


/*
 * Find the value for a given key
 *
 * Parameters:
 *   dict     The dictionary
 *   key      The key
 * 
 * Returns: The value, or INVALID_VALUE if key not found in dict
 */
CDictValueType CD_retrieve(CDict dict, CDictKeyType key);


/*
 * Delete a key from the dictionary
 *
 * Parameters:
 *   dict     The dictionary
 *   key      The key
 * 
 * Returns: None
 */
void CD_delete(CDict dict, CDictKeyType key);


/*
 * Return the load factor for the dictionary
 *
 * Parameters:
 *   dict     The dictionary
 * 
 * Returns: The current load factor, which is
 *     (num_elements + num_deleted) / (total_elements_allocated)
 */
double CD_load_factor(CDict dict);


/*
 * For debugging: Walk the dictionary and print all entries, including
 * the unused and deleted slots. 
 *
 * Parameters:
 *   dict     The dictionary
 * 
 * Returns: None
 */
void CD_print(CDict dict);


typedef void (*CD_foreach_callback)(CDictKeyType key, CDictValueType value, void *cb_data);

/*
 * Iterate through the dictionary, calling the user-specified callback
 * function for each element.  Each call to callback will be of the
 * form
 * 
 *   callback( <key>, <value>, <cb_data> )
 *
 * There is no guarantee as to the order in which the callback is
 * called.
 *
 * Parameters:
 *   dict       The dictionary
 *   callback   The function to call
 *   cb_data    Caller data to pass to the function
 * 
 * Returns: None
 */
void CD_foreach(CDict dict, CD_foreach_callback callback, void *cb_data);


#endif /* _CDICT_H_ */
