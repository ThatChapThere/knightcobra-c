#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "legal_moves.h"
#include "move_bitboards.h"

void add_legal_moves_to_node(node_type *node, moveset_type *legal_moves)
{
    position_type position = node->position;
    bool white_to_move = position.player_to_move == white_player;
    square_datum_type player_pawn = white_to_move ? white_pawn : black_pawn;
    square_datum_type player_king = player_pawn + white_king - white_pawn; 

    for(square_datum_type chessman = player_pawn; chessman <= player_king; chessman++)
    {
        bitboard_type chessman_bitboard = position.bitboards[chessman];

        /* loop through bitboard and try to find examples of the chessman */
        for(int sq = 0; sq < 64; sq++)
        {
            /* if there isn't one, skip this iteration */
            bitboard_type current_square_at_final_bit = chessman_bitboard >> (63 - sq);
            bool chessman_at_current_square = current_square_at_final_bit & 1;
            
            if(!chessman_at_current_square) continue;

            moveset_type moveset = 
            	legal_moves[chessman * NUMBER_OF_SQUARES + sq];

            for(int m = 0; m < moveset.move_count; m++)
            {
                move_type move = moveset.moves[m];
                bool is_legal = true;

                for(int c = 0; c < move.condition_count; c++)
                {
                    condition_type condition = move.conditions[c];
                    bitboard_type chessmen_on_condition_squares = position.bitboards[condition.datum] & condition.squares;
                    
                    if(condition.fill)
                    {
                        if(chessmen_on_condition_squares < condition.squares)
                        	is_legal = false;
                    }
                    else
                    {
                    	if(chessmen_on_condition_squares)
                    		is_legal = false;
					}
                }

                if(is_legal)
                {
                    node_type **child = get_next_child_of_node(node);
                    *child = malloc(sizeof(node_type));
                    position_type *new_position = &(*child)->position;
                    copy_position_to(
                        node->position,
                        new_position
                    );
                    new_position->fifty_move_counter++;
                    new_position->player_to_move ^= true;
                    bool is_capture = false;

                    for(int e = 0; e < move.effect_count; e++)
                    {
                        effect_type effect = move.effects[e];
                        if(effect.fill)
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
                        chessman == white_pawn ||
                        chessman == black_pawn ||
                        is_capture
                    ) new_position->fifty_move_counter = 0;
                }
            }
        }
    }
}

// double pointer explained:
//
// when we need data, we return it
// when we need to change data, we return a pointer to it.
//
// when we need a pointer, we return it
// when we need to change a pointer, we return a pointer to it.
node_type **get_next_child_of_node(node_type *node)
{
	return &node->children[node->child_count++];
}

void free_node(node_type *node)
{
    for(int i = 0; i < node->child_count; i++)
    {
        free_node(node->children[i]);
        free(node->children[i]);
    }
    node->child_count = 0;
}
