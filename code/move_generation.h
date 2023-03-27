#ifndef LEGAL_MOVES_H
#define LEGAL_MOVES_H

#include "display.h"

#define MAX_LEGAL_MOVES (9 * 27 + 2 * 14 + 3 * 8 + 2 * 13 - 16) /* 9Q 2R 2N 1K 2B */
#define MAX_DEPTH 30
#define MAX_TREE_SIZE MAX_LEGAL_MOVES * MAX_DEPTH

struct node
{
	struct position position;
	struct node *first_child;
	struct node *last_child;
	struct node *parent;
};

struct search_tree
{
	struct node nodes[MAX_TREE_SIZE];
	struct node *last_node;
	long long int node_counter;
};

void search_at_depth(struct search_tree *search_tree, int depth, struct moveset *legal_moves);

#endif
