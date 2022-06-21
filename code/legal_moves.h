#include "position.h"

#ifndef _LEGAL_MOVES_H_
    #define _LEGAL_MOVES_H_

    #define MAX_LEGAL_MOVES (27 * 9 + 2 * 14 + 4 * 8 + 2 * 13 - 16) /* 9Q 2R 4N/K 2B */

    typedef struct node_type node_type;

    typedef struct node_type{
        position_type position;
        int child_count;
        node_type *children[MAX_LEGAL_MOVES];
    } node_type;

    void add_legal_moves_to_node(node_type *node, moveset_type *legal_moves);
    void free_node(node_type *node);

#endif