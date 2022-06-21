#include <stdbool.h>

#ifndef _MOVE_BITBOARDS_H_
    #define _MOVE_BITBOARDS_H_

    /* all 12 pieces, plus castling, en passant and control for both sides */
    #define NUMBER_OF_SQUARE_DATA_TYPES (chessmen + 1)

    #define NUMBER_OF_CHESSMEN 12
    #define CHESSMEN_PER_SIDE 6
    #define MAX_CONDITIONS 10
    #define MAX_EFFECTS 10
    #define MAX_MOVES_PER_CHESSMAN 27
    #define NUMBER_OF_SQUARES 64

    #define MIN_WHITE_MAN_INDEX 0
    #define MAX_WHITE_MAN_INDEX 5
    #define MIN_BLACK_MAN_INDEX 6
    #define MAX_BLACK_MAN_INDEX 11

    #define is_chessman(sq_dtm) (sq_dtm <= black_king)

    /* other typedefs */
    typedef unsigned long long int bitboard_type;


    /* enums */
    typedef enum {
        white_pawn, white_rook, white_knight, white_bishop, white_queen, white_king,
        black_pawn, black_rook, black_knight, black_bishop, black_queen, black_king,
        white_enpassant, black_enpassant,
        white_castling, black_castling,
        white_castling_blockers, black_castling_blockers,
        white_control, black_control,
        white_chessmen, black_chessmen,
        chessmen
    } square_datum_type;


    /* structs */
    typedef struct {
        bool fill; /* if 0, the square must be empty for a move to be legal; if 1 the square must be filled */
        int data_count;
        square_datum_type datum; /* the datum that the square must (not) contain */
        bitboard_type squares; /* a bit with a a value of 1 indicates that a square must be checked */
    } condition_type;

    /* this makes it possible to use functions on both types, as they hold the same data */
    /* here, "fill" means fill */
    typedef condition_type effect_type;

    typedef struct {
        int condition_count;
        int effect_count;
        condition_type conditions[MAX_CONDITIONS];
        effect_type effects[MAX_EFFECTS];
    } move_type;

    typedef struct {
        int move_count;
        move_type moves[MAX_MOVES_PER_CHESSMAN];
    } moveset_type;


    /* functions */
    void generate_bitboards(moveset_type * legal_moves);

#endif