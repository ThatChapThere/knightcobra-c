#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "move_bitboards.h"

#define BOARD_WIDTH 8
#define NUMBER_OF_LINE_PIECES 6
#define NUMBER_OF_STRAIGHT_PIECES 4
#define NUMBER_OF_DIAGONAL_PIECES 4
#define LINE_MOVE_TYPES 4

/* number of chessman types per side */
#define MEN_PER_SIDE 6

bool is_rook[NUMBER_OF_CHESSMEN] = { [white_rook] = true, [black_rook] = true };
bool is_bishop[NUMBER_OF_CHESSMEN] = { [white_bishop] = true, [black_bishop] = true };

square_datum line_pieces[] = { white_rook, white_bishop, white_queen, black_rook, black_bishop, black_queen };

square_datum white_men[] = { white_pawn, white_rook, white_knight, white_bishop, white_queen, white_king };
square_datum black_men[] = { black_pawn, black_rook, black_knight, black_bishop, black_queen, black_king };

typedef enum {
    file, rank, backslash_diagonal, forwardslash_diagonal
} line_move;

/* function declarations */
void add_data_to_condition_or_effect(
    square_datum *square_data, int data_count, square_datum *condition_or_effect_data, int *condition_or_effect_data_count
);
void add_datum_to_condition_or_effect(
    square_datum the_square_datum,
    square_datum *condition_or_effect_data,
    int *condition_or_effect_data_count
);

void add_start_square_to_move(move *current_move, bitboard square, square_datum man);
void add_intermediate_squares_to_move(move *current_move, bitboard squares, square_datum man);
void add_end_square_to_move(move *current_move, bitboard square, square_datum man);

bool is_white(square_datum man);
square_datum *own_men(square_datum man);
square_datum *opponent_men(square_datum man);
bitboard get_bit_from_line_move(int f, int r, int distance, line_move line_move_type);
bitboard get_bit_from_coords(int f, int r);

/* generate the rules of chess at a specified location in memory */
void generate_bitboards(moveset *legal_moves)
{
    /* loop through board */
    /* files a to h */
    for(int f = 0; f < BOARD_WIDTH; f++)
    {
        /* ranks 8 to 1 */
        for(int r = 0; r < BOARD_WIDTH; r++)
        {
            /* rook, bishop and queen moves */
            /* loop through all line pieces */
            for(int lp = 0; lp < NUMBER_OF_LINE_PIECES; lp++)
            {
                square_datum line_piece = line_pieces[lp];
                /* get the set of moves from square (f, r) for piece "line_piece" */
                moveset *piece_square = &legal_moves[
                    /* piece */
                    NUMBER_OF_SQUARES * line_piece +
                    /* square */
                    f + (8 * r)
                ];

                /* loop through directions */
                for(line_move line_move_type = 0; line_move_type < LINE_MOVE_TYPES; line_move_type++)
                {
                    /* loop through end places */
                    for(int end = 0; end < BOARD_WIDTH; end++)
                    {
                        /* rooks can't move diagonally, vice versa for bishops */
                        bool is_diagonal = (bool) (line_move_type & backslash_diagonal & forwardslash_diagonal);
                        if(
                            (is_diagonal && is_rook[line_piece]) ||
                            (!is_diagonal && is_bishop[line_piece])
                        ) continue;

                        /* the rank is used to indicate the start, unless it's a move along a rank */
                        int start = line_move_type == rank ? f : r;
                        int move_distance = end - start;
                        int direction = abs(move_distance) / move_distance;

                        /* moves cannot start and end on the same square */
                        /* or leave the board */
                        if(!move_distance) continue;
                        if(!get_bit_from_line_move(f, r, move_distance, line_move_type)) continue;

                        /* add a new move */
                        move *current_move = &piece_square->moveset[ piece_square->move_count++ ];

                        /* find intermediate squares */
                        bitboard intermediate_squares;
                        /* distance can be both positive and negative */
                        int distance;
                        /* loop through intermediate squares */
                        for(
                            /* start by moving one square */
                            distance = direction;
                            /* don't go the full way, because captures are possible */
                            distance != move_distance;
                            distance += direction

                          /* add the current intermediate square to the intermediate squares */
                        ) intermediate_squares |= get_bit_from_line_move(f, r, distance, line_move_type);

                        add_start_square_to_move(current_move, get_bit_from_coords(f, r), line_piece);
                        add_intermediate_squares_to_move(current_move, intermediate_squares, line_piece);
                        add_end_square_to_move(current_move, get_bit_from_line_move(f, r, distance, line_move_type), line_piece);
                    }
                }
            }
        }
    }
}

/* remove the man from the square */
void add_start_square_to_move(move *current_move, bitboard square, square_datum man)
{
    effect *removal = &current_move->effects[ current_move->effect_count++ ];
    /* remove */
    removal->add_piece = false;
    /* the man */
    removal->square_data[ removal->data_count++ ] = man;
    /* from the square*/
    removal->squares = square;
}

/* add a set of squares that must be completely empty */
void add_intermediate_squares_to_move(move *current_move, bitboard squares, square_datum man)
{
    condition * clear_path = &current_move->conditions[ current_move->condition_count++ ];
    /* intermediate squares */
    clear_path->squares = squares;
    /* must be empty */
    clear_path->must_be_empty = true;
    /* of any pieces */
    add_data_to_condition_or_effect(white_men, MEN_PER_SIDE, clear_path->square_data, &clear_path->data_count);
    add_data_to_condition_or_effect(black_men, MEN_PER_SIDE, clear_path->square_data, &clear_path->data_count);
}

/* add a set of squares where all opponent pieces will be removed and the current piece will be added */
/* and no pieces of the same colour may be present */
void add_end_square_to_move(move *current_move, bitboard square, square_datum man)
{
    effect *placement = &current_move->effects[ current_move->effect_count++ ];
    /* add */
    placement->add_piece = true;
    /* the piece */
    placement->square_data[ placement->data_count++ ] = man;
    /* to the current piece */
    placement->squares = square;

    effect * capture = &current_move->effects[ current_move->effect_count++ ];
    /* remove */
    capture->add_piece = false;
    /* opponent's pieces */
    add_data_to_condition_or_effect(
        opponent_men(man), MEN_PER_SIDE, capture->square_data, &capture->data_count);
    /* from the square */
    capture->squares = square;

    condition * free_of_own_pieces = &current_move->conditions[ current_move->condition_count++ ];
    /* cannot */
    free_of_own_pieces->must_be_empty = true;
    /* move onto */
    free_of_own_pieces->squares = square;
    /* your own pieces */
    add_data_to_condition_or_effect(
        own_men(man),  MEN_PER_SIDE, free_of_own_pieces->square_data, &free_of_own_pieces->data_count);
}

/* add multiple pieces or other square data to a condition or effect */
void add_data_to_condition_or_effect(
    square_datum *square_data,
    int data_count,
    square_datum *condition_or_effect_data,
    int *condition_or_effect_data_count
){
    for(int i = 0; i < data_count; i++)
        add_datum_to_condition_or_effect(square_data[i], condition_or_effect_data, condition_or_effect_data_count);
}

void add_datum_to_condition_or_effect(
    square_datum the_square_datum,
    square_datum *condition_or_effect_data,
    int *condition_or_effect_data_count
){
    condition_or_effect_data[ *condition_or_effect_data_count++ ] = the_square_datum;
}

bool is_white(square_datum man)
{
    return man <= MAX_WHITE_MAN_INDEX;
}

square_datum *own_men(square_datum man)
{
    return is_white(man)?
        white_men:
        black_men;
}

square_datum *opponent_men(square_datum man)
{
    return is_white(man)?
        black_men:
        white_men;
}

/* return a bitboard with a 1 at the end of a line move */
bitboard get_bit_from_line_move(int f, int r, int distance, line_move line_move_type)
{
    switch (line_move_type)
    {
        /* move along a file */
        case file:
            return get_bit_from_coords(f, r + distance);
        
        /* move along a rank */
        case rank:
            return get_bit_from_coords(f + distance, r);

        /* backslash diagonal move */
        case backslash_diagonal:
            return get_bit_from_coords(f + distance, r + distance);

        /* forward slash diagonal move */
        case forwardslash_diagonal:
            /* we have to try to flip across both axes because the start square is the pivot point */
            /* "distance" has a range dependent on the start rank, it's not 0-7 like "end" */
            return get_bit_from_coords(f + distance, r - distance)?
                get_bit_from_coords(f + distance, r - distance):
                get_bit_from_coords(f - distance, r + distance);
    }
    printf("Move generation error - meaningless line piece move");
    return 0;
}

/* return a bitboard with a 1 at specified coordinates */
bitboard get_bit_from_coords(int f, int r)
{
    /* if the move is off of the board */
    if((f | r) & -8) return 0;
    /* 
     * eg. for h1: 
     * f and h are 7
     * 7 + 7 * 8 = 63
     * 63 - 63 = 0
     * correctly returns 1ul - the last bit
     */
    return 1ull << (63 - (f + 8 * r));
}