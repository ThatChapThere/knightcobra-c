#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rules.h"
#include "move_generation.h"
#include "display.h"

int main()
{
	srand(time(NULL));
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

	struct search_tree search_tree;
	search_tree.last_node = search_tree.nodes;
	search_tree.node_counter = 0;

	struct node *top_node = search_tree.nodes;
	top_node->position = test_position;
	top_node->parent = NULL;

	/* 5 not only crashes the program, it crashes the operating system */
	int max_depth = 7;

	start_time = clock();
	search_at_depth(&search_tree, max_depth, legal_moves);
	end_time = clock();

	printf(
		"%dms taken to search %d nodes at depth %d\n",
		(end_time - start_time) * 1000 / CLOCKS_PER_SEC,
		search_tree.node_counter,
		max_depth
	);

	/*
	int movecount = 0;
	while(top_node.child_count && movecount < 10000 && top_node.position.bitboards[WHITE_KING] && top_node.position.bitboards[WHITE_KING] )
	{
		movecount++;
		top_node.position = top_node.children[rand() % top_node.child_count]->position;
		top_node.child_count = 0;
		add_legal_moves_to_node(&top_node, legal_moves);
		printf("\n");
		print_position(top_node.position);
	}

	//print_position(top_node.children[9]->children[9]->children[9]->children[9]->position);
	*/

	/* end program gracefully */
	free(legal_moves);
	// getchar();
	return 0;
}
