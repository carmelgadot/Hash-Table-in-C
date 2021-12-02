#include "HashSet.h"

#define SUBTRACT_FACTOR 1
#define HASH_FUNC_FACTOR 2
#define INITIAL_IND 0
#define SUCCESS 1
#define FAIL 0
#define ERROR -1
#define INITIAL_HASH_COUNT 0
#define INITIAL_SIZE 0
#define MIN_CAPACITY 1
#define ONE_NODE 1

int check_inputs_hash_set_at (HashSet *hash_set, int value);
int clamp (size_t capacity, int index);
void resize (HashSet *hash_set, int new_capacity);
int get_index_hash_function (size_t capacity, HashFunc hash_func,
                             Value value, int i);
HashSet *get_new_hashset (HashFunc hash_func,
                          HashSetValueCpy value_cpy,
                          HashSetValueCmp value_cmp,
                          HashSetValueFree value_free, int capacity);
int check_inputs_hash_set_clear (HashSet *hash_set);

void delete_old_nodes_list (HashSet *hash_set);
Node **get_new_nodes_list (HashSet *hash_set, int new_capacity);
/**
 * Allocates dynamically new hash set.
 * @param hash_func a function which "hashes" keys.
 * @param value_cpy a function which copies Values.
 * @param value_cmp a function which compares Values.
 * @param value_free a function which frees Values.
 * @return pointer to dynamically allocated HashSet.
 * @if_fail return NULL or if one of the function pointers is null.
 */
HashSet *hash_set_alloc (
    HashFunc hash_func, HashSetValueCpy value_cpy,
    HashSetValueCmp value_cmp, HashSetValueFree value_free)
{

  if (!hash_func || !value_cpy || !value_cmp || !value_free)
    {
      return NULL;
    }

  return get_new_hashset (hash_func, value_cpy, value_cmp, value_free,
                          HASH_SET_INITIAL_CAP);

}

HashSet *get_new_hashset (HashFunc hash_func,
                          HashSetValueCpy value_cpy,
                          HashSetValueCmp value_cmp,
                          HashSetValueFree value_free, int capacity)
{
  HashSet *new_hashset = malloc (sizeof (HashSet));

  Node **new_nodes_p_arr = malloc (sizeof (Node *) * capacity);

  for (int i = INITIAL_IND; i < capacity; i++)
    {

      new_nodes_p_arr[i] = node_alloc (value_cpy, value_cmp, value_free);
    }

  new_hashset->NodesList = new_nodes_p_arr;
  new_hashset->size = INITIAL_SIZE;
  new_hashset->capacity = capacity;
  new_hashset->hash_func = hash_func;
  new_hashset->value_cpy = value_cpy;
  new_hashset->value_cmp = value_cmp;
  new_hashset->value_free = value_free;

  return new_hashset;
}

/**
 * Frees the hash set and all its elements.
 * @param p_hash_set pointer to pointer to a hash_set.
 */
void hash_set_free (HashSet **p_hash_set)
{
  if (!p_hash_set || !*p_hash_set)
    {
      return;
    }

  for (size_t i = INITIAL_IND; i < (*p_hash_set)->capacity; i++)
    {
      node_free (&((*p_hash_set)->NodesList[i]));

    }

  free ((*p_hash_set)->NodesList);
  (*p_hash_set)->NodesList = NULL;
  free (*p_hash_set);
  *p_hash_set = NULL;

}

/**
 * Inserts a new Value to the hash set.
 * The function inserts *new*, *copied*, *dynamically allocated* Value,
 * @param hash_set the hash set where we want to insert the new element
 * @param value a Value we would like to add to the hashset
 * @return returns 1 for successful insertion, 0 otherwise.
 */
int hash_set_insert (HashSet *hash_set, Value value)
{

  if (!hash_set || !value || !hash_set->hash_func || !hash_set->value_free
      || !hash_set->value_cmp
      || !hash_set->value_cpy
      || (hash_set_contains_value (hash_set, value) == SUCCESS))
    {
      return FAIL;
    }
  Value val;
  Node *first_node_hash;

  for (size_t i = INITIAL_IND; i < hash_set->capacity; i++)
    {

      int index = get_index_hash_function (hash_set->capacity,
                                           hash_set->hash_func,
                                           value, (int) i);
      if (i == INITIAL_IND)
        {
          first_node_hash = hash_set->NodesList[index];

        }
      val = hash_set->NodesList[index]->data;
      if (!val) // empty place
        {
          if (set_node_data (hash_set->NodesList[index], value) == FAIL)
            {
              return FAIL;
            }
          hash_set->size++;
          first_node_hash->hashCount++;
          // check if resize is needed
          if (hash_set_get_load_factor (hash_set) >= HASH_SET_MAX_LOAD_FACTOR)
            {
              resize (hash_set,
                      (int) hash_set->capacity *
                      HASH_SET_GROWTH_FACTOR);

            }

          return SUCCESS;
        }

    }
  return FAIL;

}

/**
 * The function checks if the given value exists in the hash set.
 * @param hash_set a hash set.
 * @param value the value to be checked.
 * @return 1 if the value is in the hash set, 0 otherwise.
 */
int hash_set_contains_value (HashSet *hash_set, Value value)
{
  Node *first_node_hash;

  if (!hash_set || !value)
    {
      return FAIL;
    }

  int hash_count = ONE_NODE;
  // hashing the value
  for (int i = INITIAL_IND; i <= hash_count; i++)
    {

      int index = get_index_hash_function (hash_set->capacity,
                                           hash_set->hash_func, value, i);

      if (i == INITIAL_IND)
        {
          first_node_hash = hash_set->NodesList[index];
          hash_count = first_node_hash->hashCount;
          if (hash_count == INITIAL_HASH_COUNT)
            {
              return FAIL;
            }
        }
      if (hash_set->NodesList[index]->data == NULL)
        {

          if (first_node_hash->hashCount != ONE_NODE)
            {
              hash_count++;
            }

          continue;
        }
      // the value is in the node
      if (check_node (hash_set->NodesList[index], value) == SUCCESS)
        {
          return SUCCESS;
        }

    }
  return FAIL;

}

/**
 * get index hash
 * @param capacity the capacity of the hashset
 * @param hash_func a function which "hashes" keys.
 * @param value the value to hash
 * @param i the index of hashing loop , the i try to hash
 * @return index hash
 */
int get_index_hash_function (size_t capacity, HashFunc hash_func, Value value,
                             int i)
{
  return clamp (capacity, (int) hash_func (value)
                          + (i + (i * i)) / HASH_FUNC_FACTOR);
}

/**
 * The function erases a Value.
 * @param hash_set a hash set.
 * @param value the value to be erased from the hashtable
 * @return 1 if the erasing was done successfully, 0 otherwise.
 */
int hash_set_erase (HashSet *hash_set, Value value)
{
  Node *first_node_hash;
  if (!hash_set || !value || !hash_set->hash_func || !hash_set->value_cpy
      || !hash_set->value_free ||
      !hash_set->value_cmp)
    {
      return FAIL;
    }

  int hash_count = ONE_NODE;
  for (int i = INITIAL_IND; i <= hash_count; i++)
    {

      int index = get_index_hash_function (hash_set->capacity,
                                           hash_set->hash_func, value, i);
      if (i == INITIAL_IND)
        {
          first_node_hash = hash_set->NodesList[index];
          hash_count = first_node_hash->hashCount;

        }

      // the value is in the node
      if (check_node (hash_set->NodesList[index], value) == SUCCESS)
        {
          clear_node (hash_set->NodesList[index]);
          hash_set->size--;
          first_node_hash->hashCount--;
          // check if resize is needed
          if (hash_set_get_load_factor (hash_set) <= HASH_SET_MIN_LOAD_FACTOR
              && hash_set->capacity != MIN_CAPACITY)
            {
              resize (hash_set,
                      ((int) hash_set->capacity /
                       HASH_SET_GROWTH_FACTOR));

            }
          return SUCCESS;
        }

      if (hash_set->NodesList[index]->data == NULL)
        {
          if (first_node_hash->hashCount != ONE_NODE)
            {
              hash_count++;
            }
        }

    }
  return FAIL; // the value is not in the hashset
}

/**
 * resize the Nodes list of the hashset
 * @param hash_set a hash set
 * @param new_capacity the new capacity of the hashset
 */
void resize (HashSet *hash_set, int new_capacity)
{
  Node **new_nodes_p_arr = get_new_nodes_list (hash_set, new_capacity);

  // go over the old hashset and hash each value to the new hashset
  for (size_t i = INITIAL_IND; i < hash_set->capacity; i++)
    {
      // empty node we do not need to copy
      Value old_data = hash_set->NodesList[i]->data;
      // empty node we do not need to copy
      if (old_data == NULL)
        {
          continue;
        }

      Node *first_node_hash;
      Value value;
      //  hash value to the new hashset
      for (int j = INITIAL_IND; j < new_capacity; j++)
        {
          int index = get_index_hash_function (new_capacity,
                                               hash_set->hash_func,
                                               old_data, j);
          if (j == INITIAL_IND)
            {
              // save pointer to the first hashed node
              first_node_hash = new_nodes_p_arr[index];

            }
          value = new_nodes_p_arr[index]->data;
          if (!value) // empty place
            {

              // copy tha value
              if (set_node_data (new_nodes_p_arr[index], old_data) == FAIL)
                {
                  return;
                }
              first_node_hash->hashCount++;
              break;
            }

        }
    }

  delete_old_nodes_list (hash_set);
  hash_set->capacity = (size_t) new_capacity;
  hash_set->NodesList = new_nodes_p_arr;
}

/**
 *  initial memory allocation for nodes pointers array
 * @param hash_set a hash_set
 * @param new_capacity the capacity of the nodes list
 * @return nodes pointers array
 */
Node **get_new_nodes_list (HashSet *hash_set, int new_capacity)
{//  initial memory allocation for array of Node pointers
  Node **new_nodes_p_arr = malloc (sizeof (Node *) * new_capacity);

  //  initial memory allocation for nodes
  for (int i = INITIAL_IND; i < new_capacity; i++)
    {

      new_nodes_p_arr[i] = node_alloc (hash_set->value_cpy,
                                       hash_set->value_cmp,
                                       hash_set->value_free);
    }
  return new_nodes_p_arr;
}

/**
 * delete the old nodes list
 * @param hash_set a hash set
 */
void delete_old_nodes_list (HashSet *hash_set)
{
  for (size_t i = INITIAL_IND; i < hash_set->capacity; i++)
    {
      node_free (&(hash_set->NodesList[i]));

    }

  free (hash_set->NodesList);
}

/**
 * Clamps hashing indices to fit within the current table capacity
 * @param hash_set
 * @param index the index before clamping.
 * @return an index properly clamped.
 */
int clamp (size_t capacity, int index)
{
  return (index & ((int) capacity - SUBTRACT_FACTOR));
}

/**
 * This function returns the load factor of the hash set.
 * @param hash_set a hash set.
 * @return the hash set's load factor, -1 if the function failed.
 */
double hash_set_get_load_factor (HashSet *hash_set)
{
  if (!hash_set) // checks if the input is legal
    {
      return ERROR;
    }
  return ((double) hash_set->size) / ((double) hash_set->capacity);
}

/**
 * This function deletes all the values in the hash set.
 * @param hash_set a hash set to be cleared.
 */
void hash_set_clear (HashSet *hash_set)
{
  if (check_inputs_hash_set_clear (hash_set) == FAIL)
    {
      return;
    }

  // go over the nodes list and delete the data in them
  for (size_t i = INITIAL_IND; i < hash_set->capacity; i++)
    {
      if (!hash_set->NodesList[i]->elem_free_func)
        {
          return;
        }
      clear_node (hash_set->NodesList[i]);
      // initialize the hash count of a node
      hash_set->NodesList[i]->hashCount = INITIAL_HASH_COUNT;
    }
  hash_set->size = INITIAL_SIZE; // initialize the size of the hashset

}

/**
 * checks if the input is legal
 * @param hash_set the hashset
 * @return 1 if the input is legal ,else return 0.
 */
int check_inputs_hash_set_clear (HashSet *hash_set)
{
  if (!hash_set || !hash_set->NodesList)
    {
      return FAIL;
    }
  return SUCCESS;
}

/**
 * This function gets the element in the hashset hashed to the indexed value
 * @param hash_set the hashset
 * @param value the index of the node in the hashtable that we would like to
 * have its value
 * @return the element in the hashset hashed to the indexed value
 * NULL in case of an error or an empty node or an unexpected value
 */
Value hash_set_at (HashSet *hash_set, int value)
{

  if (check_inputs_hash_set_at (hash_set, value) == FAIL)
    {
      return NULL;
    }
  // the element in the hashset hashed to the indexed value
  return hash_set->NodesList[value]->data;
}

/**
 * checks if the inputs are legal
 * @param hash_set the hashset
 * @param value the index of the node in the hashtable that we would like to
 * have its value
 * @return 1 if the inputs are legal ,else return 0.
 */
int check_inputs_hash_set_at (HashSet *hash_set, int value)
{
  if (!hash_set || value < INITIAL_IND || (size_t) value >= hash_set->capacity
      || !hash_set->NodesList || !hash_set->NodesList[value]
      || !(hash_set->NodesList[value]->data))
    {
      return FAIL;
    }
  return SUCCESS;
}