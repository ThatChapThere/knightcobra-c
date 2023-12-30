#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rules.h"
#include "move_generation.h"
#include "position.h"
#include "time_formatting.h"
#include "fen.h"
#include "display.h"

int main()
{
	/* rules of chess - left 1d for performance */
	/* note that 2d arrays in C are actually 1d anyway, sort of, so maybe fix this */
	struct moveset *legal_moves = malloc(
		sizeof(struct moveset) *
		NUMBER_OF_CHESSMEN *
		NUMBER_OF_SQUARES);

	clock_t start_time = clock();
	generate_bitboards(legal_moves);
	clock_t end_time = clock();

	printf("%d ms taken to generate bitboards\n", (end_time - start_time) * 1000 / CLOCKS_PER_SEC);

	struct position starting_position;
	set_position_from_fen(&starting_position, STARTING_FEN);

	print_position(starting_position);

	int max_depth = 5;
	for(int depth = 1; depth <= max_depth; depth++)
	{
		struct search_tree search_tree;
		search_tree.last_node = search_tree.nodes;
		search_tree.node_counter = 0;

		struct node *top_node = search_tree.nodes;
		top_node->position = starting_position;
		top_node->parent = NULL;

		start_time = clock();
		search_at_depth(&search_tree, depth, legal_moves);
		end_time = clock();

		int clock_time = end_time - start_time;
		int ms_passed = 1000 * clock_time / CLOCKS_PER_SEC;
		int knps = CLOCKS_PER_SEC * search_tree.node_counter / clock_time / 1000;

		print_time(clock_time);
		printf(
			" taken to search %llu nodes at depth %d - %d kn/s\n",
			search_tree.node_counter,
			depth,
			knps
		);
	}

	free(legal_moves);
	return 0;
}
