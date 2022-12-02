#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "move_bitboards.h"
#include "legal_moves.h"
#include "position.h"

int main()
{
    time_t start_time = clock();
    /* rules of chess - left 1d for performance */
    moveset_type *legal_moves = malloc(
        sizeof(moveset_type) *
        NUMBER_OF_CHESSMEN *
        NUMBER_OF_SQUARES
    );
    generate_bitboards(legal_moves);
    /* set a timer for benchmarking */
    time_t end_time = clock();
    printf("%d ms taken to generate bitboards\n\n", (int) difftime(end_time, start_time));
    position_type test_position;
    set_position_from_fen(&test_position, STARTING_FEN);
    print_position(test_position);
    node_type test_node;
    test_node.position = test_position;
    test_node.child_count = 0;
    add_legal_moves_to_node(&test_node, legal_moves);
    printf("\n%d legal moves available\n\n", test_node.child_count);
    for(int i = 0; i < test_node.child_count; i++)
	{
        print_position(test_node.children[i]->position);
        add_legal_moves_to_node(test_node.children[i], legal_moves);
        for(int j = 0; j < test_node.children[i]->child_count; j++)
		{
			print_position(test_node.children[i]->children[j]->position);
		}
	}
    free_node(&test_node);
    free(legal_moves);
    getchar();
    return 0;
}
