#include <stdio.h>
#include "move_bitboards.h"

#define NUMBER_OF_STRAIGHT_PIECES 4
#define NUMBER_OF_DIAGONAL_PIECES 4
#define MEN_PER_SIDE 6


int straight_pieces[] = { white_rook, white_queen, black_rook, black_queen };
int diagonal_pieces[] = { white_bishop, white_queen, black_bishop, black_queen };

int white_men[] = { white_pawn, white_rook, white_knight, white_bishop, white_queen, white_king };
int black_men[] = { black_pawn, black_rook, black_knight, black_bishop, black_queen, black_king };


void generate_bitboards(struct moves * legal_moves)
{
    /* initialise all pointers needed in the loop */
    struct moves * square;
    struct move * move;
    struct move_condition * condition;
    struct move_effect * effect;
    int * condition_pieces;
    int * effect_pieces;

    unsigned long int start_square_bitboard;
    unsigned long int end_square_bitboard;

    /* loop through board */
    for(int i = 0; i < 8; i++) /* files a to h */
    {
        for(int j = 0; j< 8; j++) /* ranks 8 to 1 */
        {
            for(int end = 0; end < 8; end++) /* end places */
            {
                for(
                    int piece_type_index = 0;
                    piece_type_index < NUMBER_OF_STRAIGHT_PIECES;
                    piece_type_index++
                )
                {
                    square = &legal_moves[
                        NUMBER_OF_SQUARES * straight_pieces[piece_type_index] +
                        (i + (8 * j)) 
                    ];
                    
                    /* sideways means along a rank, otherwise up a file */
                    for(int sideways = 0; sideways < 2; sideways++)
                    {
                        move = &square->moves[ square->move_count++ ]; /* add a new move */

                        /* remove the piece from its original square */
                        effect = &move->move_effects[ move->move_condition_count++ ]; /* add a new effect */
                        /* removal by default */
                        effect->pieces[ effect->piece_count++ ] = straight_pieces[piece_type_index]; /* remove the current piece */

                        /* add the piece to its destination square */
                        effect = &move->move_effects[ move->move_condition_count++ ]; /* add a new effect */
                        effect->add_piece = 1; /* specify addition */
                        effect->pieces[ effect->piece_count++ ] = straight_pieces[piece_type_index]; /* add the current piece */

                        /* remove all other pieces from the destination square */
                        effect = &move->move_effects[ move->move_condition_count++ ]; /* add a new effect */
                        /* removal by default */
                        for(int k = 0; k < MEN_PER_SIDE; k++)
                        {
                            effect->pieces[ effect->piece_count++ ] = 
                                straight_pieces[piece_type_index] < MIN_BLACK_MAN_INDEX ? /* is the piece being moved white? */
                                    black_men[k]: /* if so capture black pieces */
                                    white_men[k]; /* otherwise capture white pieces */
                        }
                    }
                }
            }
        }
    }
    return;
};