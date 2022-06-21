#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "move_bitboards.h"

#define BOARD_WIDTH 8
#define NUMBER_OF_LINE_PIECES 6
#define NUMBER_OF_STRAIGHT_PIECES 4
#define NUMBER_OF_DIAGONAL_PIECES 4
#define LINE_MOVE_TYPES 4

#define WHITE_KINGSQUARE get_bit_from_coords(4, 7)
#define WHITE_CASTLE_QUEENSIDE_ENDSQUARE get_bit_from_coords(2, 7)
#define WHITE_CASTLE_QUEENSIDE_ROOKSQUARE get_bit_from_coords(7, 7)
#define WHITE_CASTLE_KINGSIDE_ENDSQUARE get_bit_from_coords(6, 7)
#define WHITE_CASTLE_KINGSIDE_ROOKSQUARE get_bit_from_coords(7, 7)

#define BLACK_KINGSQUARE get_bit_from_coords(4, 0)
#define BLACK_CASTLE_QUEENSIDE_ENDSQUARE get_bit_from_coords(2, 0)
#define BLACK_CASTLE_QUEENSIDE_ROOKSQUARE get_bit_from_coords(0, 0)
#define BLACK_CASTLE_KINGSIDE_ENDSQUARE get_bit_from_coords(6, 0)
#define BLACK_CASTLE_KINGSIDE_ROOKSQUARE get_bit_from_coords(7, 0)

#define pawn_on_start_row(pawn, r) (is_white(pawn) ? r == 6 : r == 1)
#define pawn_about_to_promote(pawn, r) (is_white(pawn) ? r == 1 : r == 6)
#define pawn_row_invalid(r) (r == 0 || r == 7)
#define can_enpassant(pawn, r) (is_white(pawn) ? r == 3 : r == 4)

#define is_white(chessman) (chessman <= MAX_WHITE_MAN_INDEX)

typedef enum{ pawn, rook, knight, bishop, queen, king } chessman_type;

typedef enum {
    file, rank, backslash_diagonal, forwardslash_diagonal
} line_move;

typedef enum { queenside, kingside } castling_side;

/* function declarations */
void generate_line_piece_bitboards (moveset_type *legal_moves, int f, int r, bitboard_type startsquare);
void generate_knight_bitboards     (moveset_type *legal_moves, int f, int r, bitboard_type startsquare);
void generate_king_bitboards       (moveset_type *legal_moves, int f, int r, bitboard_type startsquare);
void generate_pawn_bitboards       (moveset_type *legal_moves, int f, int r, bitboard_type startsquare);

void add_single_effect(move_type *move, bitboard_type squares, square_datum_type chessman, bool is_addition);
void add_datum_to_squares(move_type *move, bitboard_type squares, square_datum_type chessman);
void remove_datum_from_squares(move_type *move, bitboard_type squares, square_datum_type chessman);

void add_single_condition(move_type *move, bitboard_type squares, square_datum_type chessman, bool must_be_filled);
void squares_must_have(move_type *move, bitboard_type squares, square_datum_type chessman);
void squares_must_be_free_of(move_type *move, bitboard_type squares, square_datum_type chessman);

void add_intermediate_squares_to_move(move_type *move, bitboard_type squares);
void add_end_square_to_move(move_type *move, bitboard_type squares, square_datum_type chessman);
void add_through_squares_to_castling(move_type *move, bitboard_type squares, square_datum_type king);

bool is_an_example_of_chessman(chessman_type kind_of_chessman, square_datum_type chessman);

condition_type *get_next_condition(move_type *move);
effect_type *get_next_effect(move_type *move);

move_type *get_next_move_of_moveset(moveset_type *moveset);

square_datum_type own_chessmen(square_datum_type chessman);
square_datum_type opponent_control(square_datum_type chessman);
square_datum_type opponent_chessmen(square_datum_type chessman);
square_datum_type own_castling_blockers(square_datum_type king);

square_datum_type *get_next_datum_from_condition_or_effect(condition_type *condition_or_effect);

bitboard_type get_castling_throughsquares(bitboard_type startsquare, castling_side side);
bitboard_type get_bit_from_line_move(int f, int r, int distance, line_move line_move_type);
bitboard_type get_bit_from_coords(int f, int r);

moveset_type *get_moveset_from_coordinates_and_chessman(moveset_type *legal_moves, int f, int r, square_datum_type chessman);

/* line piece indeces */
const square_datum_type line_pieces[] = { white_rook, white_bishop, white_queen, black_rook, black_bishop, black_queen };

/* function bodies */
void generate_bitboards(moveset_type *legal_moves)
{
    for(int f = 0; f < BOARD_WIDTH; f++) /* a-h */
    for(int r = 0; r < BOARD_WIDTH; r++) /* 8-1 */
    {
        bitboard_type startsquare = get_bit_from_coords(f, r);

        generate_line_piece_bitboards (legal_moves, f, r, startsquare);
        generate_knight_bitboards     (legal_moves, f, r, startsquare);
        generate_king_bitboards       (legal_moves, f, r, startsquare);
        generate_pawn_bitboards       (legal_moves, f, r, startsquare);
    }
}

void generate_line_piece_bitboards(moveset_type *legal_moves, int f, int r, bitboard_type startsquare)
{
    for(int lp = 0; lp < NUMBER_OF_LINE_PIECES; lp++)
    {
        square_datum_type line_piece = line_pieces[lp];

        /* get the set of moves from square (f, r) for piece "line_piece" */
        
        moveset_type *moveset = 
            get_moveset_from_coordinates_and_chessman(legal_moves, f, r, line_piece);

        /* loop through directions */
        for(line_move line_move_type = 0; line_move_type < LINE_MOVE_TYPES; line_move_type++)
        {
            /* rooks can't move diagonally, vice versa for bishops */
            bool is_diagonal = (bool) (line_move_type & backslash_diagonal & forwardslash_diagonal);
            if(
                (is_diagonal && is_an_example_of_chessman(rook, line_piece)) ||
                (!is_diagonal && is_an_example_of_chessman(bishop, line_piece))
            ) continue;

            /* loop through end places */
            for(int end = 0; end < BOARD_WIDTH; end++)
            {
                /* the rank is used to indicate the start, unless it's a move along a rank */
                int start = line_move_type == rank ? f : r;
                int move_distance = end - start;
                int direction = abs(move_distance) / move_distance;

                /* moves cannot start and end on the same square */
                /* or leave the board */
                if(!move_distance) continue;
                if(!get_bit_from_line_move(f, r, move_distance, line_move_type)) continue;

                /* add a new move */
                move_type *move = get_next_move_of_moveset(moveset);

                /* find intermediate squares */
                bitboard_type intermediate_squares;

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

                remove_datum_from_squares(move, startsquare, line_piece);
                add_intermediate_squares_to_move(move, intermediate_squares);
                add_end_square_to_move(move, get_bit_from_line_move(f, r, distance, line_move_type), line_piece);

                /* make castling impossible when a rook moves away from its start square */
                if(is_an_example_of_chessman(rook, line_piece))
                {
                    if(is_white(line_piece))
                    {
                        if(startsquare == WHITE_CASTLE_KINGSIDE_ROOKSQUARE)
                            remove_datum_from_squares(move, WHITE_CASTLE_KINGSIDE_ENDSQUARE, white_castling);

                        if(startsquare == WHITE_CASTLE_QUEENSIDE_ROOKSQUARE)
                            remove_datum_from_squares(move, WHITE_CASTLE_QUEENSIDE_ENDSQUARE, white_castling);
                    /* black rooks */
                    }else{
                        if(startsquare == BLACK_CASTLE_KINGSIDE_ROOKSQUARE)
                            remove_datum_from_squares(move, BLACK_CASTLE_KINGSIDE_ENDSQUARE, white_castling);

                        if(startsquare == BLACK_CASTLE_QUEENSIDE_ROOKSQUARE)
                            remove_datum_from_squares(move, BLACK_CASTLE_QUEENSIDE_ENDSQUARE, white_castling);
                    }
                }
            }
        }
    }
}

void generate_knight_bitboards(moveset_type *legal_moves, int f, int r, bitboard_type startsquare)
{
    for(square_datum_type knight = white_knight; knight <= black_knight; knight += CHESSMEN_PER_SIDE)
    {
        moveset_type *moveset = 
            get_moveset_from_coordinates_and_chessman(legal_moves, f, r, knight);

        int is_to_right, is_down, is_mostly_file;

        for(int knight_move = 0; knight_move < 8; knight_move++)
        {
            is_to_right = knight_move & 1,
            is_down = knight_move & 2,
            is_mostly_file = knight_move & 4;

            bitboard_type endsquare = get_bit_from_coords(
                f + ((is_to_right ? 1 : -1) * (is_mostly_file ? 2 : 1)),
                r + ((is_down ? 1 : -1) * (is_mostly_file ? 1 : 2))
            );

            if(endsquare)
            {
                move_type *move = get_next_move_of_moveset(moveset);

                remove_datum_from_squares(move, startsquare, knight);
                add_end_square_to_move(move, endsquare, knight);
            }
        }
    }
}

void generate_king_bitboards(moveset_type *legal_moves, int f, int r, bitboard_type startsquare)
{
    for(square_datum_type king = white_king; king <= black_king; king += CHESSMEN_PER_SIDE)
    {
        moveset_type *moveset =
            get_moveset_from_coordinates_and_chessman(legal_moves, f, r, king);

        int file_movement, rank_movement;

        /* regular king moves */
        for(int kingmove = 0; kingmove < 9; kingmove++)
        {
            file_movement = (kingmove % 3) - 1;
            rank_movement = (kingmove / 3) - 1;

            if( !(file_movement || rank_movement) ) continue;

            bitboard_type endsquare = get_bit_from_coords(
                f + file_movement,
                r + rank_movement
            );

            if(endsquare)
            {
                move_type *move = get_next_move_of_moveset(moveset);

                /* move king */
                remove_datum_from_squares(move, startsquare, king);
                add_end_square_to_move(move, endsquare, king);
                
                /* can no longer castle */
                if(startsquare == WHITE_KINGSQUARE && king == white_king)
                {
                    remove_datum_from_squares(move, WHITE_CASTLE_KINGSIDE_ENDSQUARE, white_castling);
                    remove_datum_from_squares(move, WHITE_CASTLE_QUEENSIDE_ENDSQUARE, white_castling);
                }
                else if(startsquare == BLACK_KINGSQUARE && king == black_king)
                {
                    remove_datum_from_squares(move, BLACK_CASTLE_KINGSIDE_ENDSQUARE, black_castling);
                    remove_datum_from_squares(move, BLACK_CASTLE_QUEENSIDE_ENDSQUARE, black_castling);
                }

                /* cannot move into check */
                /* you can't legally move into check by any means regardless */
                /* this line exists purely for performance */
                squares_must_be_free_of(move, endsquare, opponent_control(king));
            }
        }

        /* castling */
        if(
            (startsquare == WHITE_KINGSQUARE && king == white_king) ||
            (startsquare == BLACK_KINGSQUARE && king == black_king)
        )
        {
            for(castling_side side = queenside; side <= kingside; side++)
            {
                move_type *castling_move = get_next_move_of_moveset(moveset);

                const bitboard_type endsquare =
                    is_white(king) ?
                        side == kingside ?
                            WHITE_CASTLE_KINGSIDE_ENDSQUARE:
                            WHITE_CASTLE_QUEENSIDE_ENDSQUARE:
                        side == kingside ?
                            BLACK_CASTLE_KINGSIDE_ENDSQUARE:
                            BLACK_CASTLE_QUEENSIDE_ENDSQUARE;
                
                const bitboard_type rook_startsquare =
                    is_white(king) ?
                        side == kingside ?
                            WHITE_CASTLE_KINGSIDE_ROOKSQUARE:
                            WHITE_CASTLE_QUEENSIDE_ROOKSQUARE:
                        side == kingside ?
                            BLACK_CASTLE_KINGSIDE_ROOKSQUARE:
                            BLACK_CASTLE_QUEENSIDE_ROOKSQUARE;

                const bitboard_type rook_endsquare =
                    side == kingside ?
                        endsquare << 1:
                        endsquare >> 1;
                
                const square_datum_type own_castling_possible =
                    is_white(king) ? white_castling : black_castling;

                /* move king */
                remove_datum_from_squares(castling_move, startsquare, king);
                add_datum_to_squares(castling_move, endsquare, king);

                /* move rook */
                remove_datum_from_squares(castling_move, rook_startsquare, white_rook);
                add_datum_to_squares(castling_move, rook_endsquare, white_rook);

                /* add conditions for squares being passed through */
                add_through_squares_to_castling(
                    castling_move,
                    get_castling_throughsquares(startsquare, kingside),
                    king
                );

                /* cannot castle out of check */
                squares_must_be_free_of(castling_move, startsquare, opponent_control(king));

                /* must still be allowed to castle */
                squares_must_have(castling_move, endsquare, own_castling_possible);
            }
        }
    }
}

void generate_pawn_bitboards(moveset_type *legal_moves, int f, int r, bitboard_type startsquare)
{
    for(square_datum_type pawn = white_pawn; pawn <= black_pawn; pawn += CHESSMEN_PER_SIDE)
    {
        if(pawn_row_invalid(r)) break;

        moveset_type *moveset =
            get_moveset_from_coordinates_and_chessman(legal_moves, f, r, pawn);

        const int rank_direction = is_white(pawn) ? -1 : 1;

        /* non promotion */
        if(!pawn_about_to_promote(pawn, r))
        {
                /* single step pawn move */
            bitboard_type single_step_endsquare = get_bit_from_coords(f, r + rank_direction);
            move_type *single_step = get_next_move_of_moveset(moveset);
            
            /* move pawn */
            remove_datum_from_squares(single_step, startsquare, pawn);
            add_datum_to_squares(single_step, single_step_endsquare, pawn);

            /* require emptiness at endsquare */
            add_intermediate_squares_to_move(single_step, single_step_endsquare);

                /* double step pawn move */
            if(pawn_on_start_row(pawn, r))
            {
                bitboard_type double_step_endsquare = get_bit_from_coords(f, r + rank_direction * 2);
                move_type *double_step = get_next_move_of_moveset(moveset);
                square_datum_type own_enpassant = 
                    is_white(pawn) ?
                        white_enpassant:
                        black_enpassant;

                /* move_type pawn */
                remove_datum_from_squares(double_step, startsquare, pawn);
                add_datum_to_squares(double_step, double_step_endsquare, pawn);
                
                /* intermediate and end square must be empty */
                add_intermediate_squares_to_move(
                    double_step,
                    single_step_endsquare | double_step_endsquare
                );

                /* add possiblity of en passant */
                add_datum_to_squares(double_step, single_step_endsquare, own_enpassant);
            }

                /* captures including en passant */
            for(int capture_direction = -1; capture_direction <= 1; capture_direction += 2)
            {
                bitboard_type endsquare = get_bit_from_coords(f + capture_direction, r + rank_direction);
                if(!endsquare) continue;

                    /* normal captures */
                move_type *pawn_capture = get_next_move_of_moveset(moveset);

                /* pick up pawn */
                remove_datum_from_squares(pawn_capture, startsquare, pawn);
                /* capture and place */
                add_end_square_to_move(pawn_capture, endsquare, pawn);
                /* must be a capture */
                squares_must_have(pawn_capture, endsquare, opponent_chessmen(pawn));

                    /* en passant */
                if( !can_enpassant(pawn, r) ) continue;

                square_datum_type opponent_enpassant = 
                    is_white(pawn) ?
                        black_enpassant:
                        white_enpassant;
                
                square_datum_type opponent_pawn = 
                    is_white(pawn) ?
                        black_pawn:
                        black_pawn;
                
                bitboard_type enpassant_capture_square = get_bit_from_coords(f + capture_direction, r);
                
                move_type *enpassant = get_next_move_of_moveset(moveset);

                /* move_type pawn */
                remove_datum_from_squares(enpassant, startsquare, pawn);
                add_datum_to_squares(enpassant, endsquare, pawn);

                /* require en passant to be possible */
                squares_must_have(enpassant, endsquare, opponent_enpassant);

                /* perform capture */
                remove_datum_from_squares(enpassant, enpassant_capture_square, opponent_pawn);
            }
        }

        /* promotion */
        else
        {
            for(square_datum_type p = white_rook; p <= white_queen; p++)
            {
                square_datum_type promotion_piece = p + (is_white(pawn) ? 0 : CHESSMEN_PER_SIDE);
                
                    /* single step pawn move_type */
                bitboard_type single_step_endsquare = get_bit_from_coords(f, r + rank_direction);
                move_type *single_step = get_next_move_of_moveset(moveset);
                
                /* promote pawn */
                remove_datum_from_squares(single_step, startsquare, pawn);
                add_datum_to_squares(single_step, single_step_endsquare, promotion_piece);

                /* require emptiness at endsquare */
                add_intermediate_squares_to_move(single_step, single_step_endsquare);

                    /* captures */
                for(int capture_direction = -1; capture_direction <= 1; capture_direction += 2)
                {
                    bitboard_type endsquare = get_bit_from_coords(f + capture_direction, r + rank_direction);
                    if(!endsquare) continue;

                        /* normal captures */
                    move_type *pawn_capture = get_next_move_of_moveset(moveset);

                    /* pick up pawn */
                    remove_datum_from_squares(pawn_capture, startsquare, pawn);
                    /* capture and promote */
                    add_end_square_to_move(pawn_capture, endsquare, promotion_piece);
                    /* must be a capture */
                    squares_must_have(pawn_capture, endsquare, opponent_chessmen(pawn));
                }
            }
        }
    }
}

void add_single_effect(move_type *move, bitboard_type squares, square_datum_type chessman, bool is_addition)
{
    effect_type *effect = get_next_effect(move);

    effect->datum = chessman;

    effect->fill = is_addition;
    effect->squares = squares;
}

void add_single_condition(move_type *move, bitboard_type squares, square_datum_type chessman, bool must_be_filled)
{
    condition_type *condition = get_next_condition(move);

    condition->datum = chessman;
    
    condition->fill = must_be_filled;
    condition->squares = squares;
}

void remove_datum_from_squares(move_type *move, bitboard_type squares, square_datum_type chessman)
    { add_single_effect(move, squares, chessman, false); }

void add_datum_to_squares(move_type *move, bitboard_type squares, square_datum_type chessman)
    { add_single_effect(move, squares, chessman, true); }

void squares_must_be_free_of(move_type *move, bitboard_type squares, square_datum_type chessman)
    { add_single_condition(move, squares, chessman, false); }

void squares_must_have(move_type *move, bitboard_type squares, square_datum_type chessman)
    { add_single_condition(move, squares, chessman, true); }

/* add a set of squares that must be completely empty */
void add_intermediate_squares_to_move(move_type *move, bitboard_type squares)
    { squares_must_be_free_of(move, squares, chessmen); }

void add_through_squares_to_castling(move_type *move, bitboard_type squares, square_datum_type king)
    { squares_must_be_free_of(move, squares, own_castling_blockers(king)); }

bitboard_type get_castling_throughsquares(bitboard_type startsquare, castling_side side)
{
    return  
    side == kingside ?
    (startsquare >> 1) | (startsquare >> 2) :
    (startsquare << 1) | (startsquare << 2);
}

/* add a set of squares where:
     * opponent pieces will be removed 
     * the current piece will be added 
     * no pieces of the same colour may be present */
void add_end_square_to_move(move_type *move, bitboard_type squares, square_datum_type chessman)
{
    add_datum_to_squares(move, squares, chessman);
    remove_datum_from_squares(move, squares, opponent_chessmen(chessman));
    squares_must_be_free_of(move, squares, own_chessmen(chessman));
}

effect_type *get_next_effect(move_type * move){return
    &move->effects[ move->effect_count++ ];}

condition_type *get_next_condition(move_type *move){return
    &move->conditions[ move->condition_count++ ];}

bool is_an_example_of_chessman(chessman_type that_kind_of_chessman, square_datum_type chessman)
{
    return
    chessman                     == that_kind_of_chessman ||
    chessman - CHESSMEN_PER_SIDE == that_kind_of_chessman;
}

square_datum_type own_chessmen(square_datum_type chessman){return
    is_white(chessman) ? white_chessmen : black_chessmen;}

square_datum_type opponent_control(square_datum_type chessman){return
    is_white(chessman) ? white_control : black_control;}

square_datum_type opponent_chessmen(square_datum_type chessman){return
    is_white(chessman) ? black_chessmen : white_chessmen;}

square_datum_type own_castling_blockers(square_datum_type king)
{
    return
    is_white(king) ? white_castling_blockers : black_castling_blockers;
}

/* return a bitboard with a 1 at the end of a line move */
bitboard_type get_bit_from_line_move(int f, int r, int distance, line_move line_move_type)
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
    return 0;
}

/* return a bitboard with a 1 at specified coordinates */
bitboard_type get_bit_from_coords(int f, int r)
{
    return
    /* if the move is off of the board */
    ((f | r) & -8) ? 0 :

    /* 
     * eg. for h1: 
     * f and r are 7
     * 7 + 7 * 8 = 63
     * 63 - 63 = 0
     * correctly returns 1ull - the last bit
     */
    1ull << (63 - (f + 8 * r));
}

move_type *get_next_move_of_moveset(moveset_type *moveset){return
    &moveset->moves[ moveset->move_count++ ];}

moveset_type *get_moveset_from_coordinates_and_chessman(moveset_type * legal_moves, int f, int r, square_datum_type chessman)
{
    return 
    &legal_moves[
        chessman * NUMBER_OF_SQUARES +
        f + (8 * r)
    ];
}