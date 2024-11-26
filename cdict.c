
/*
 * cdict.c
 *
 * Dictionary based on a hash table utilizing open addressing to
 * resolve collisions.
 *
 * Author: <Uwase Pauline>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "cdict.h"

#define DEBUG

#define DEFAULT_DICT_CAPACITY 8
#define REHASH_THRESHOLD 0.6

typedef enum
{
  SLOT_UNUSED = 0,
  SLOT_IN_USE,
  SLOT_DELETED
} CDictSlotStatus;

struct _hash_slot
{
  CDictSlotStatus status;
  CDictKeyType key;
  CDictValueType value;
};

struct _dictionary
{
  unsigned int num_stored;
  unsigned int num_deleted;
  unsigned int capacity;
  struct _hash_slot *slot;
};

static unsigned int _CD_hash(CDictKeyType str, unsigned int capacity)
{
  unsigned int x;
  unsigned int len = 0;

  if (!str)
    return 0; // Handle NULL input

  // Calculate string length
  for (const char *p = str; *p; p++)
    len++;

  if (len == 0)
    return 0;

  // Initial hash value
  const char *p = str;
  x = (unsigned int)*p << 7;

  for (int i = 0; i < len; i++)
    x = (1000003 * x) ^ (unsigned int)*p++;

  x ^= (unsigned int)len;

  return x % capacity;
}

static void _CD_rehash(CDict dict)
{
  assert(dict);

  unsigned int new_capacity = dict->capacity * 2;
  struct _hash_slot *old_slots = dict->slot;

  struct _hash_slot *new_slots = calloc(new_capacity, sizeof(struct _hash_slot));
  if (!new_slots)
    return; // Allocation failed, skip rehashing

  dict->slot = new_slots;
  new_slots = NULL;
  free(new_slots);
  dict->num_stored = 0;
  dict->num_deleted = 0;
  unsigned int old_capacity = dict->capacity;
  dict->capacity = new_capacity;

  // Reinsert elements from old slots into new slots
  for (unsigned int i = 0; i < old_capacity; i++)
  {
    if (old_slots[i].status == SLOT_IN_USE)
    {
      CD_store(dict, old_slots[i].key, old_slots[i].value);
      free((void*)old_slots[i].key);
    }
  }

  free(old_slots);
}

CDict CD_new()
{
  CDict dict = malloc(sizeof(struct _dictionary));
  assert(dict);

  dict->num_stored = 0;
  dict->num_deleted = 0;
  dict->capacity = DEFAULT_DICT_CAPACITY;
  dict->slot = malloc(DEFAULT_DICT_CAPACITY * sizeof(struct _hash_slot));

  assert(dict->slot);

  for (unsigned int i = 0; i < DEFAULT_DICT_CAPACITY; i++)
  {
    dict->slot[i].status = SLOT_UNUSED;
    dict->slot[i].key = "";
    dict->slot[i].value = 0.0;
  }

  return dict;
}

void CD_free(CDict dict)
{
  if (!dict)
    return;

  for(int i = 0; i < dict->capacity; i++) {
    if(dict->slot[i].status == SLOT_IN_USE) {
      free((void*)dict->slot[i].key);
    }
  }

  free(dict->slot);
  free(dict);
}

unsigned int CD_size(CDict dict)
{
  assert(dict);

#ifdef DEBUG
  // iterate across slots, counting number of keys found
  unsigned int used = 0;
  unsigned int deleted = 0;
  for (unsigned int i = 0; i < dict->capacity; i++)
  {
    if (dict->slot[i].status == SLOT_IN_USE)
      used++;
    else if (dict->slot[i].status == SLOT_DELETED)
      deleted++;
  }

  assert(used == dict->num_stored);
  assert(deleted == dict->num_deleted);
#endif

  return dict->num_stored;
}

unsigned int CD_capacity(CDict dict)
{
  assert(dict);
  return dict->capacity;
}

bool CD_contains(CDict dict, CDictKeyType key)
{
  assert(dict);
  assert(key);

  unsigned int index = _CD_hash(key, dict->capacity);

  for (unsigned int i = 0; i < dict->capacity; i++)
  {
    unsigned int probe = (index + i) % dict->capacity;
    if (dict->slot[probe].status == SLOT_UNUSED)
    {
      return false;
    }
    if (dict->slot[probe].status == SLOT_IN_USE &&
        strcmp(dict->slot[probe].key, key) == 0)
    {
      return true;
    }
  }
  return false;
}

void CD_store(CDict dict, CDictKeyType key, CDictValueType value)
{
  assert(dict);
  assert(key);
  assert(value);


  printf("Storing key: %s with value: %.2f\n", key, value);  // Debug print
  unsigned int index = _CD_hash(key, dict->capacity);

  for (unsigned int i = 0; i < dict->capacity; i++)
  {
    unsigned int probe = (index + i) % dict->capacity;

    // If updating existing key
    if (dict->slot[probe].status == SLOT_IN_USE &&
        strcmp(dict->slot[probe].key, key) == 0)
    {
      dict->slot[probe].value = value;
      return;
    }
    else if (dict->slot[probe].status != SLOT_UNUSED)
    {
      continue;
    }
    else
    {
      // New key insertion
      dict->slot[probe].status = SLOT_IN_USE;
      dict->slot[probe].key = strdup(key);
      dict->num_stored++;
      dict->slot[probe].value = value;

      if (CD_load_factor(dict) > REHASH_THRESHOLD)
      {
        _CD_rehash(dict);
      }
      return;
    }
  }
}

CDictValueType CD_retrieve(CDict dict, CDictKeyType key)
{
  assert(dict);
  assert(key);

  unsigned int index = _CD_hash(key, dict->capacity);
  for (unsigned int i = 0; i < dict->capacity; i++)
  {
    unsigned int probe = (index + i) % dict->capacity;
    if (dict->slot[probe].status == SLOT_UNUSED)
    {
      return NAN;
    }
    if (dict->slot[probe].status == SLOT_IN_USE &&
        strcmp(dict->slot[probe].key, key) == 0)
    {
      return dict->slot[probe].value;
    }
  }
  return NAN;
}

void CD_delete(CDict dict, CDictKeyType key)
{
  assert(dict);
  assert(key);

  unsigned int index = _CD_hash(key, dict->capacity);
  for (unsigned int i = 0; i < dict->capacity; i++)
  {
    unsigned int probe = (index + i) % dict->capacity;
    if (dict->slot[probe].status == SLOT_UNUSED)
    {
      return;
    }
    if (dict->slot[probe].status == SLOT_IN_USE &&
        strcmp(dict->slot[probe].key, key) == 0)
    {
      dict->slot[probe].status = SLOT_DELETED;
      dict->num_stored--;
      dict->num_deleted++;
      return;
    }
  }
}

double CD_load_factor(CDict dict)
{
  assert(dict);
  return (double)(dict->num_stored + dict->num_deleted) / dict->capacity;
}

void CD_print(CDict dict)
{
  assert(dict);

  printf("Dictionary contents (capacity=%u, stored=%u, deleted=%u load_factor=%.2f):\n",
         dict->capacity, dict->num_stored, dict->num_deleted, CD_load_factor(dict));
  for (unsigned int i = 0; i < dict->capacity; i++)
  {
    if (dict->slot[i].status == SLOT_IN_USE)
    {
      printf("Slot %u: key='%s', value='%f'\n", i + 1, dict->slot[i].key, dict->slot[i].value);
    }
    else if (dict->slot[i].status == SLOT_DELETED)
    {
      printf("Slot %u: DELETED\n", i + 1);
    }
    else
    {
      printf("Slot %u: unused\n", i + 1);
    }
  }
}

void CD_foreach(CDict dict, CD_foreach_callback callback, void *cb_data)
{
  assert(dict);
  assert(callback);

  for (unsigned int i = 0; i < dict->capacity; i++)
  {
    if (dict->slot[i].status == SLOT_IN_USE)
    {
      callback(dict->slot[i].key, dict->slot[i].value, cb_data);
    }
  }
}
