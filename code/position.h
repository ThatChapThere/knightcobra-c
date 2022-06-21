#include "move_bitboards.h"

#ifndef _POSITION_H_
    #define _POSITION_H_

    #define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

    typedef enum { black_player, white_player } player;

    typedef struct
    {
        player player_to_move;
        int fifty_move_counter;
        bitboard_type bitboards[NUMBER_OF_SQUARE_DATA_TYPES];
    } position_type;

    void set_position_from_fen(position_type *current_position, char *fen);
    void copy_position_to(position_type position_to_copy, position_type *position_to_copy_to);
    void print_position(position_type position);

#endif