#include "legal_moves.h"
#include "move_bitboards.h"
#include "stdbool.h"

void add_legal_moves_to_node(node_type *node, moveset_type *legal_moves)
{
    position_type position = node->position;

    /* for every chessman of the player to move */
    for(square_datum_type cm = white_pawn; cm <= white_king; cm++)
    {
        square_datum_type chessman = 
            cm + 
            (
                position.player_to_move == white_player ?
                0: 
                CHESSMEN_PER_SIDE
            );
        
        bitboard_type chessman_bitboard = position.bitboards[chessman];

        /* loop through bitboard and try to find examples of the chessman */
        for(int sq = 0; sq < 64; sq++)
        {
            /* if there isn't one, skip this iteration */
            if(!((chessman_bitboard >> (63 - sq)) & 1)) continue;

            moveset_type moveset = (&legal_moves[chessman * NUMBER_OF_SQUARES])[sq];

            for(int m = 0; m < moveset.move_count; m++)
            {
                move_type move = moveset.moves[m];
                bool is_legal = true;

                /* ensure all conditions are met */
                for(int c = 0; c < move.condition_count; c++)
                {
                    condition_type condition = move.conditions[c];
                    
                    if(condition.fill)
                    {
                        /* if the overlap between the condition and reality less than the condition */
                        /* ie. not all required squares are filled */
                        if(
                            (position.bitboards[condition.datum] & condition.squares) !=
                            condition.squares
                        ) is_legal = false;
                    }
                    else
                    {
                        /* if there is overlap between the condition and reality */
                        /* ie. any required squares are filled */
                        if(position.bitboards[condition.datum] & condition.squares)
                            is_legal = false;
                    }

                    if(is_legal)
                    {
                        node_type **child = get_next_child_of_node(node);
                        *child = malloc(sizeof(node));
                        position_type *new_position = &(*child)->position;
                        copy_position_to(
                            node->position,
                            new_position
                        );
                        new_position->fifty_move_counter++;
                        new_position->player_to_move ^= 1;

                        bool is_capture = false;

                        for(int e = 0; e < move.effect_count; e++)
                        {
                            effect_type effect = move.effects[e];

                            if(effect.fill)
                                new_position->bitboards[effect.datum] |= effect.squares;
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
}

node_type **get_next_child_of_node(node_type *node)
    { return &node->children[ node->child_count++ ]; }

void free_node(node_type *node)
{
    for(int i = 0; i < node->child_count; i++)
    {
        free_node(node->children[i]);
        free(node->children[i]);
    }
    node->child_count = 0;
}