#ifndef _LEGAL_MOVES_H_
#define _LEGAL_MOVES_H_

#include "position.h"

#define MAX_LEGAL_MOVES (9 * 27 + 2 * 14 + 3 * 8 + 2 * 13 - 16) /* 9Q 2R 2N 1K 2B */

typedef struct node_type node_type;

typedef struct node_type{
	position_type position;
	int child_count;
	node_type *children[MAX_LEGAL_MOVES];
} node_type;

node_type **get_next_child_of_node(node_type *node);
void add_legal_moves_to_node(node_type *node, moveset_type *legal_moves);
void free_node(node_type *node);

#endif
