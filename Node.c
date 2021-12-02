#include "Node.h"

#define SUCCESS 1
#define FAIL 0
#define ERROR -1
#define INITIAL_HASH_COUNT 0

int check_inputs_set_node_data (Node *node, void *value);
int check_inputs_get_hash_count (Node *node);
int check_inputs_clear_node (Node *node);
int check_inputs_node_alloc (NodeElemCpy elem_copy_func,
                             NodeElemCmp elem_cmp_func,
                             NodeElemFree elem_free_func);

/**
 * Allocates dynamically new Node element.
 * @param elem_copy_func func which copies the element stored in the Node
 * (returns
 * dynamically allocated copy).
 * @param elem_cmp_func func which is used to compare elements stored in the
 * Node.
 * @param elem_free_func func which frees elements stored in the Node.
 * @return pointer to dynamically allocated Node.
 * @if_fail return NULL (it returns null also if one of the function pointers
 * is null).
 * You can assume memory allocation success
 */
Node *node_alloc (NodeElemCpy elem_copy_func, NodeElemCmp elem_cmp_func,
                  NodeElemFree elem_free_func)
{
  if (check_inputs_node_alloc (elem_copy_func, elem_cmp_func,
                               elem_free_func) == FAIL)
    {
      return NULL;
    }

  Node *new_node = malloc (sizeof (Node));

  // initial node
  new_node->data = NULL;
  new_node->hashCount = INITIAL_HASH_COUNT;
  new_node->elem_copy_func = elem_copy_func;
  new_node->elem_cmp_func = elem_cmp_func;
  new_node->elem_free_func = elem_free_func;

  return new_node;
}

/**
 * checks that the functions in the input are not NULL
 * @param elem_copy_func func which copies the element stored in the Node
 * (returns
 * @param elem_cmp_func func which is used to compare elements stored in the
 * Node.
 * @param elem_free_func func which frees elements stored in the Node.
 * @return
 */
int check_inputs_node_alloc (NodeElemCpy elem_copy_func,
                             NodeElemCmp elem_cmp_func,
                             NodeElemFree elem_free_func)
{
  if (!elem_copy_func || !elem_cmp_func || !elem_free_func)
    {
      return FAIL;
    }
  return SUCCESS;
}

/**
 * Frees a Node and the elements the Node itself allocated.
 * @param p_Node pointer to dynamically allocated pointer to Node.
 */
void node_free (Node **p_Node)
{
  if (!p_Node || !*p_Node || !(*p_Node)->elem_free_func)
    {
      return;
    }
  clear_node (*p_Node);

  free (*p_Node);
  (*p_Node) = NULL;

}

/**
 * Gets a value and checks if the value is in the Node.
 * @param Node a pointer to Node.
 * @param value the value to look for.
 * @return 1 if the value is in the Node, 0 if no such value in the Node.
 * Returns -1  if something went wrong during the check
 * You cannot assume anything about the input.
 */
int check_node (Node *node, void *value)
{
  if (!node || !value || !node->elem_cmp_func)
    {
      return ERROR;
    }

  if (!node->data) // empty node
    {
      return FAIL;
    }
  if (node->elem_cmp_func (node->data, value)
      == SUCCESS) //if the value is in the Node
    {
      return SUCCESS;
    }
  return FAIL;
}

/**
 * Adds a new value to the data of the Node.
 * @param Node a pointer to Node.
 * @param value the value to be added to the Node.
 * @return 1 if the adding has been done successfully, 0 otherwise.
 */
int set_node_data (Node *node, void *value)
{
  if (check_inputs_set_node_data (node, value) == FAIL)
    {
      return FAIL;
    }

  Value new_value = node->elem_copy_func (value);

  if (node->data) // the node is not empty
    {
      clear_node (node);
    }
  node->data = new_value; // put the value in the node

  return SUCCESS;
}

int check_inputs_set_node_data (Node *node, void *value)
{
  if (!value || !node || !node->elem_copy_func || !node->elem_free_func)
    {
      return FAIL;
    }
  return SUCCESS;
}

/**
 * Deletes the data in the Node.
 * @param Node Node a pointer to Node.
 */
void clear_node (Node *node)
{
  if (check_inputs_clear_node (node) == FAIL)
    {
      return;
    }

  node->elem_free_func (&(node->data));

}

/**
 * checks if the input node is legal
 * @param node Node a pointer to Node.
 * @return 1 if the input node is legal ,else return 0.
 */
int check_inputs_clear_node (Node *node)
{
  if (!node || !node->elem_free_func || !node->data)
    {
      return FAIL;
    }
  return SUCCESS;
}

/**  
 * Gets the number of data values in the hashtable that would have been hashed
 * to this node
 * @param node The current node
 * @return the number of data in the hashtable that would have been hashed
 * to this node or -1 in case of illegal input;
 * You cannot assume anything about the input.
 */
int get_hash_count (Node *node)
{
  if (check_inputs_get_hash_count (node) == FAIL)
    {
      return ERROR;
    }
  return node->hashCount;
}

/**
 * checks if the input node is legal
 * @param node Node a pointer to Node.
 * @return 1 if the input node is legal ,else return 0.
 */
int check_inputs_get_hash_count (Node *node)
{
  if (!node)
    {
      return FAIL;
    }
  return SUCCESS;
}