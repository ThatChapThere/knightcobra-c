#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "move_bitboards.h"
#include "legal_moves.h"
#include "position.h"

int main()
{
    /* set a timer for benchmarking */
    time_t start_time = clock();

    /* rules of chess - left 1d for performance */
    /* note that 2d array in C are actually 1d anyway, sort of, so maybe fix this */
    struct moveset *legal_moves = malloc(
        sizeof(struct moveset) *
        NUMBER_OF_CHESSMEN *
        NUMBER_OF_SQUARES);
    generate_bitboards(legal_moves);
    time_t end_time = clock();
    printf("%d ms taken to generate bitboards\n\n", (int) difftime(end_time, start_time));

    /* set the startion position */
    struct position test_position;
    set_position_from_fen(&test_position, STARTING_FEN);
    print_position(test_position);
    struct node test_node;
    test_node.position = test_position;
    test_node.child_count = 0;

    /* calculate legal moves */
    int positions = 0;
    add_legal_moves_to_node(&test_node, legal_moves);
    printf("\n%d legal moves available\n\n", test_node.child_count);
    for(int i = 0; i < test_node.child_count; i++)
	{
        //print_position(test_node.children[i]->position);
        add_legal_moves_to_node(test_node.children[i], legal_moves);
        positions++;
        for(int j = 0; j < test_node.children[i]->child_count; j++)
		{
			add_legal_moves_to_node(test_node.children[i]->children[j], legal_moves);
			//print_position(test_node.children[i]->children[j]->position);
			for(int k = 0; k < test_node.children[i]->children[j]->child_count; k++)
			{
				print_position(test_node.children[i]->children[j]->children[k]->position);
				positions++;
			}
		}
	}
	printf("%d positions found\n", positions);

	/* end program gracefully */
    free_node(&test_node);
    free(legal_moves);
    getchar();
    return 0;
}
