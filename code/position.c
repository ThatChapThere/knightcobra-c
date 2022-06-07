#include "position.h"

void copy_position_to(position_type position_to_copy, position_type *position_to_copy_to)
{
    position_to_copy_to->fifty_move_counter = position_to_copy.fifty_move_counter;
    position_to_copy_to->player_to_move = position_to_copy.player_to_move;
    for(int i = 0; i < NUMBER_OF_SQUARE_DATA_TYPES; i++)
    {
        position_to_copy_to->bitboards[i] = position_to_copy.bitboards[i];
    }
}