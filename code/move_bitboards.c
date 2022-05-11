#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "move_bitboards.h"

#define BOARD_WIDTH 8
#define NUMBER_OF_LINE_PIECES 6
#define NUMBER_OF_STRAIGHT_PIECES 4
#define NUMBER_OF_DIAGONAL_PIECES 4
#define MEN_PER_SIDE 6
#define LINE_MOVE_TYPES 4

int line_pieces[] = { white_rook, white_bishop, white_queen, black_rook, black_bishop, black_queen };

bool rooks[NUMBER_OF_CHESSMEN] = { [white_rook] = true, [black_rook] = true};
bool bishops[NUMBER_OF_CHESSMEN] = { [white_bishop] = true, [black_bishop] = true};

int white_men[NUMBER_OF_SQUARE_DATA_TYPES] = { white_pawn, white_rook, white_knight, white_bishop, white_queen, white_king };
int black_men[NUMBER_OF_SQUARE_DATA_TYPES] = { black_pawn, black_rook, black_knight, black_bishop, black_queen, black_king };

typedef enum { file, rank, backslash_diagonal, forwardslash_diagonal }
    line_move;

void generate_bitboards(moves * legal_moves)
{
    /* loop through board */
    for(int f = 0; f < BOARD_WIDTH; f++) /* files a to h */
    {
        for(int r = 0; r < BOARD_WIDTH; r++) /* ranks 8 to 1 */
        {
            for(int end = 0; end < BOARD_WIDTH; end++) /* end places */
            {
                /* rook bishop and queen moves */
                for(line_move line_move_type = 0; line_move_type < LINE_MOVE_TYPES; line_move_type++)
                {
                    /* loop through all line square_data */
                    for(
                        int line_piece_index = 0;
                        line_piece_index < NUMBER_OF_LINE_PIECES;
                        line_piece_index++
                    )
                    {
                        int is_diagonal = line_move_type & backslash_diagonal & forwardslash_diagonal;
                        int is_fslash_or_rank = line_move_type & rank & forwardslash_diagonal;

                        /* the rank is used to indicate the start, unless it's a move along a rank */
                        int start = line_move_type == rank ? f : r;
                        int move_distance = end - start;

                        if(!move_distance) continue; /* moves cannot start and end on the same square */
                        if(!get_bit_from_line_move(f, r, move_distance, line_move_type)) continue; /* if the move is off of the board */

                        if(
                            (is_diagonal && rooks[line_pieces[line_piece_index]]) ||
                            (! is_diagonal && bishops[line_pieces[line_piece_index]])
                        ) continue;

                        moves * piece_square = &legal_moves[
                            NUMBER_OF_SQUARES * line_pieces[line_piece_index] + /* piece */
                            f + (8 * r) /* square */
                        ];

                        move * piece_move = &piece_square->moves[ piece_square->move_count++ ]; /* add a new move */

                        /* remove the piece from its original square */
                        move_effect * removal = &piece_move->move_effects[ piece_move->move_effect_count++ ]; /* add a new effect */
                            /* removal by default */
                        removal->square_data[ removal->data_count++ ] = line_pieces[line_piece_index]; /* remove the current piece */

                        /* add the piece to its destination square */
                        move_effect * placement = &piece_move->move_effects[ piece_move->move_effect_count++ ]; /* add a new effect */
                        placement->add_piece = true; /* specify addition */
                        placement->square_data[ placement->data_count++ ] = line_pieces[line_piece_index]; /* add the current piece */

                        /* remove all opponent pieces from the destination square */
                        move_effect * capture = &piece_move->move_effects[ piece_move->move_effect_count++ ]; /* add a new effect */
                            /* removal by default */
                        add_data_to_condition_or_effect(
                            line_pieces[line_piece_index] < MIN_BLACK_MAN_INDEX ? /* is the piece being moved white? */
                                black_men: /* if so capture black pieces */
                                white_men, /* otherwise capture white pieces */
                            MEN_PER_SIDE,
                            capture->square_data,
                            &capture->data_count
                        );

                        /* require a clear path */
                        move_condition * intermediate_squares = &piece_move->move_conditions[ piece_move->move_condition_count++ ];
                        
                        int direction = abs(move_distance) / move_distance;

                        int distance;
                        for(distance = 1; distance < move_distance; distance++)
                        {
                            intermediate_squares->squares |= get_bit_from_line_move(f, r, distance, line_move_type);
                        }
                        intermediate_squares->must_be_empty = true;

                        add_data_to_condition_or_effect(white_men, MEN_PER_SIDE, intermediate_squares->square_data, &intermediate_squares->data_count);
                        add_data_to_condition_or_effect(black_men, MEN_PER_SIDE, intermediate_squares->square_data, &intermediate_squares->data_count);

                        /* cannot capture your own pieces */
                        move_condition * end_square = &piece_move->move_conditions[ piece_move->move_condition_count++ ];
                        end_square->squares = get_bit_from_line_move(f, r, distance, line_move_type);
                        end_square->must_be_empty = true;

                        add_data_to_condition_or_effect(
                            line_pieces[line_piece_index] < MIN_BLACK_MAN_INDEX ?
                                black_men:
                                white_men,
                            MEN_PER_SIDE,
                            end_square->square_data,
                            &end_square->data_count
                        );
                    }
                }
            }
        }
    }
    return;
};

bitboard get_bit_from_line_move(int f, int r, int distance, line_move line_move_type)
{
    switch (line_move_type)
    {
        case file: /* move along a file */
            return get_bit_from_coords(f, r + distance);

        case rank: /* move along a rank */
            return get_bit_from_coords(f + distance, r);

        case backslash_diagonal: /* backslash diagonal move */
            return get_bit_from_coords(f + distance, r + distance);

        case forwardslash_diagonal: /* forward slash bishop move */
            return get_bit_from_coords(f + distance, r - distance);
    }
}

bitboard get_bit_from_coords(int f, int r)
{
    if((f | r) & -8) return 0; /* if the move is off of the board */
    /* 
     * eg. for h1: 
     * f and h are 7
     * 7 + 7 * 8 = 63
     * 63 - 63 = 0
     * correctly returns 1ul - the last bit
     */
    return 1ul << (63 - (f + 8 * r)); 
}

void add_data_to_condition_or_effect(
    square_data_type * square_data,
    int data_count,
    square_data_type * condition_or_effect_data,
    int * condition_or_effect_data_count
){
    for(int i = 0; i < data_count; i++)
    {
        condition_or_effect_data[ * condition_or_effect_data_count++ ] = square_data[i];
    }
}