#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "legal_moves.h"
#include "move_bitboards.h"

void add_legal_moves_to_node(struct node *node, struct moveset *legal_moves)
{
	struct position position = node->position;
	bool white_to_move = position.player_to_move == WHITE_PLAYER;
	enum square_datum player_pawn = white_to_move ? WHITE_PAWN : BLACK_PAWN;
	enum square_datum player_king = player_pawn + WHITE_KING - WHITE_PAWN; 

	for(enum square_datum chessman = player_pawn; chessman <= player_king; chessman++)
	{
		type_bitboard chessman_bitboard = position.bitboards[chessman];

		/* loop through bitboard and try to find examples of the chessman */
		for(int sq = 0; sq < 64; sq++)
		{
			/* if there isn't one, skip this iteration */
			type_bitboard current_square_at_final_bit = chessman_bitboard >> (63 - sq);
			bool chessman_at_current_square = current_square_at_final_bit & 1;

			if(!chessman_at_current_square) continue;

			struct moveset moveset = legal_moves[chessman * NUMBER_OF_SQUARES + sq];

			for(int m = 0; m < moveset.move_count; m++)
			{
				struct move move = moveset.moves[m];
				bool is_legal = true;

				for(int c = 0; c < move.condition_count; c++)
				{
					struct condition condition = move.conditions[c];
					type_bitboard chessmen_on_condition_squares = position.bitboards[condition.datum] & condition.squares;

					if(condition.must_contain)
					{
						if(chessmen_on_condition_squares < condition.squares) is_legal = false;
					}
					else
					{
						if(chessmen_on_condition_squares) is_legal = false;
					}
				}

				if(is_legal)
				{
					struct node **child = get_next_child_of_node(node);
					*child = malloc(sizeof(struct node));
					struct position *new_position = &(*child)->position;
					copy_position_to(
						node->position,
						new_position
					);
					new_position->fifty_move_counter++;
					new_position->player_to_move ^= true;
					bool is_capture = false;

					for(int e = 0; e < move.effect_count; e++)
					{
						struct effect effect = move.effects[e];
						if(effect.adds)
						{
							new_position->bitboards[effect.datum] |= effect.squares;
						}
						else
						{
							new_position->bitboards[effect.datum] &= ~effect.squares;
							if(is_chessman(effect.datum)) is_capture = true;
						}
					}

					if(
						chessman == WHITE_PAWN ||
						chessman == BLACK_PAWN ||
						is_capture
					) new_position->fifty_move_counter = 0;
				}
			}
		}
	}
}

/*
 * double pointer explained:
 *
 * when we need data, we return it
 * when we need to change data, we return a pointer to it.
 *
 * when we need a pointer, we return it
 * when we need to change a pointer, we return a pointer to it.
 */
struct node **get_next_child_of_node(struct node *node)
{
	return &node->children[node->child_count++];
}

void free_node(struct node *node)
{
	for(int i = 0; i < node->child_count; i++)
	{
		free_node(node->children[i]);
		free(node->children[i]);
	}
	node->child_count = 0;
}
