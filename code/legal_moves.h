#ifndef LEGAL_MOVES_H
#define LEGAL_MOVES_H

#include "position.h"

#define MAX_LEGAL_MOVES (9 * 27 + 2 * 14 + 3 * 8 + 2 * 13 - 16) /* 9Q 2R 2N 1K 2B */

struct node
{
	struct position position;
	int child_count;
	struct node *children[MAX_LEGAL_MOVES];
};

struct node **get_next_child_of_node(struct node *node);
void add_legal_moves_to_node(struct node *node, struct moveset *legal_moves);
void free_node(struct node *node);
void search_at_depth(struct node *node, struct moveset *legal_moves, int depth);

#endif
