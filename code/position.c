#include <stdio.h>
#include "position.h"
#include "move_bitboards.h"

#define A8 (1ull << 63)

const char chessman_characters[] = "PRNBQKprnbqk";

void copy_position_to(position_type position_to_copy, position_type *position_to_copy_to)
{
    position_to_copy_to->fifty_move_counter = position_to_copy.fifty_move_counter;
    position_to_copy_to->player_to_move = position_to_copy.player_to_move;

    for(int i = 0; i < NUMBER_OF_SQUARE_DATA_TYPES; i++)
        position_to_copy_to->bitboards[i] = position_to_copy.bitboards[i];
}

void set_position_from_fen(position_type *position, char *fen)
{
    for(int cm = 0; cm < NUMBER_OF_SQUARE_DATA_TYPES; cm++)
        position->bitboards[cm] = 0ull;

    int i = 0;
    int square_offset;
    bitboard_type square = A8;
    char next_character = fen[0];

    /* the positions of the chessmen */
    for(; next_character != ' '; i++)
    {
        next_character = fen[i];

        for(int cm = 0; cm < NUMBER_OF_CHESSMEN; cm++)
            if(chessman_characters[cm] == next_character)
                position->bitboards[cm] |= square;

        square >>= 
            ('0' < next_character && next_character < '9') ?
                next_character - '0' :
            next_character == '/' ?
                0 : 1;
    }

    /* side to move */
    for(; fen[i] != ' '; i++)
        position->player_to_move = fen[i] == 'w';

    for(square_datum_type chessman = white_pawn; chessman <= black_king; chessman++)
        if(chessman <= white_king)
            position->bitboards[white_chessmen] |= position->bitboards[chessman];
        else
            position->bitboards[black_chessmen] |= position->bitboards[chessman];
    
    position->bitboards[chessmen] = position->bitboards[white_chessmen] | position->bitboards[black_chessmen];
    return;

    /* castling */
    for(; next_character != ' '; i++);

    /* en passant */
    for(; next_character != ' '; i++);

    /* half moves */
    for(; next_character != ' '; i++);

    /* full moves */
    for(; next_character; i++);
}

void print_position(position_type position)
{
    char output[73];
    output[72] = '\0';
    for(int i = 0; i < 72; i++)
        output[i] = (i + 1) % 9 ? '_' : '\n';
    
    int i = 0;
    for(bitboard_type square = A8; square; square >>= 1, i++)
        for(int cm = 0; cm < NUMBER_OF_CHESSMEN; cm++)
            if(position.bitboards[cm] & square)
                output[i + i / 8] = chessman_characters[cm];

    printf("\n%s\n", output);
}