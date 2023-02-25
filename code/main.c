#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "move_bitboards.h"
#include "legal_moves.h"
#include "position.h"

int main()
{
	/* set a timer for benchmarking */
	clock_t start_time = clock();

	/* rules of chess - left 1d for performance */
	/* note that 2d array in C are actually 1d anyway, sort of, so maybe fix this */
	struct moveset *legal_moves = malloc(
		sizeof(struct moveset) *
		NUMBER_OF_CHESSMEN *
		NUMBER_OF_SQUARES);
	generate_bitboards(legal_moves);
	clock_t end_time = clock();
	printf("%d ms taken to generate bitboards\n\n", (end_time - start_time) * 1000 / CLOCKS_PER_SEC);

	/* set the startion position */
	struct position test_position;
	set_position_from_fen(&test_position, STARTING_FEN);
	print_position(test_position);
	struct node test_node;
	test_node.position = test_position;
	test_node.child_count = 0;

	const int predetermined_move_count = 5;
	int predetermined_move = 0;
	//int predetermined_moves[] = {9, 9, 16, 15, 22, 23};
	int predetermined_moves[] = {1, 0, 0, 1, 0};

	//printf("\e[0;32m");
	/* calculate legal moves */
	int choice = 0;
	//return 0;
	while(true)
	{
		add_legal_moves_to_node(&test_node, legal_moves);

		puts("==================================================================================================");
		for(int i = 0; i < test_node.child_count; i++)
		{
			printf("%d.\n", i);
			print_position(test_node.children[i]->position);
		}
		while(true)
		{
			if(predetermined_move < predetermined_move_count) choice = predetermined_moves[predetermined_move++];
			else scanf("%d", &choice);
			if(0 <= choice && choice < test_node.child_count)
			{
				struct position current_position = test_node.children[choice]->position;
				free_node(&test_node);
				copy_position_to(current_position, &test_node.position);
				break;
			}
		}
	}

	/* end program gracefully */
	free_node(&test_node);
	free(legal_moves);
	/* getchar(); */
	return 0;
}
