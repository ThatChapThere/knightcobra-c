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

	/* set the starting position */
	struct position test_position;
	set_position_from_fen(&test_position, STARTING_FEN);
	print_position(test_position);
	struct node test_node;
	test_node.position = test_position;
	test_node.child_count = 0;

	/* end program gracefully */
	free_node(&test_node);
	free(legal_moves);
	// getchar();
	return 0;
}
