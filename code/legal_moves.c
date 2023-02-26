#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "legal_moves.h"
#include "move_bitboards.h"

void set_control_bitboards_for_node(struct node *node, struct moveset *legal_moves);

void search_at_depth(struct node *node, struct moveset *legal_moves, int depth)
{
	if(depth == 0) return;

	add_legal_moves_to_node(node, legal_moves);
	search_at_depth(node, legal_moves, depth - 1);
}

void add_legal_moves_to_node(struct node *node, struct moveset *legal_moves)
{
	struct position position = node->position;
	bool white_to_move = position.player_to_move == WHITE_PLAYER;
	enum square_datum player_pawn = white_to_move ? WHITE_PAWN : BLACK_PAWN;
	enum square_datum player_king = player_pawn + WHITE_KING - WHITE_PAWN; 

	set_control_bitboards_for_node(node, legal_moves);

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

/* this will set the control for both sides
 * it might be more optimal to make this function technically inaccurate as we only need the opponent's control
 * for move generation purposes, but for now we'll let it do both sides for move evaluation purposes 
 *
 * there are some things to think about here
 * this function serves basically one purpose only as far as move generation is concerned: making sure you
 * don't castle through check. It unforunately can't prevent the player from moving into check since you
 * can open a discovery on yourself. It's still more optimal to check control for every king move though, just
 * so that fewer non-moves are generated
 *
 * the issue I'm concerned about is whether or not order matters in terms of which side gets their moves set first
 * this isn't a problem with non-king moves since all non-king moves ignore the control bitboards entirely
 *
 * however, there could be an issue where the control is set for one king and not the other for a square they could
 * both reach
 * if both kings can reach a square, first the white king is said to be in control of that square
 * then a condition is violated for the black king and the control is not set
 * then the moves are generated for white, and black doesn't nominally control that square, and white is allowd to 
 * make an illegal move
 * 
 * even more strangely this would not be picked up by the king capture code I plan to add to cover other ways of
 * putting youself in check, in the specific case that the king is "defended" by another piece
 *
 * it has also just occured to me that naively using captures as a measure of control would let the king capture
 * defended pieces
 * this can be fixed by only considering certain conditions and not others
 */
void set_control_bitboards_for_node(struct node *node, struct moveset *legal_moves)
{
	struct position *position = &node->position;

	position->bitboards[WHITE_CONTROL] =
	position->bitboards[BLACK_CONTROL] = 0ull;

	for(enum square_datum chessman = WHITE_PAWN; chessman <= BLACK_KING; chessman++)
	{
		type_bitboard chessman_bitboard = position->bitboards[chessman];
		enum square_datum control = is_white(chessman) ? WHITE_CONTROL : BLACK_CONTROL;

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
				bool is_controlled = true;

				for(int c = 0; c < move.condition_count; c++)
				{
					struct condition condition = move.conditions[c];
					type_bitboard chessmen_on_condition_squares = position->bitboards[condition.datum] & condition.squares;

					/* we only care about move blockers to prevent control 
					 * control includes defended pieces, so we don't care about single colour conditions that prevent
					 * self-captures
					 * we must ignore control conditions for kings so that two kings both prevent
					 * each other from moving to a square
					 * an exception is pawns don't control the square in front of them
					 * however, we can just go by capture effects to prevent this problem */
					if(!condition.must_contain && condition.datum == CHESSMEN)
					{
						if(chessmen_on_condition_squares) is_controlled = false;
					}
				}

				if(is_controlled)
				{
					for(int e = 0; e < move.effect_count; e++)
					{
						struct effect effect = move.effects[e];
						/* if a capture is "possible" here */
						if(!effect.adds)
							position->bitboards[control] |= effect.squares;
					}
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
