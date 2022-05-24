#include <stdbool.h>

#ifndef _MOVE_BITBOARDS_H_

    #define _MOVE_BITBOARDS_H_

    /* all 12 pieces, plus castling, en passant and control for both sides */
    #define NUMBER_OF_SQUARE_DATA_TYPES 18

    #define NUMBER_OF_CHESSMEN 12
    #define MAX_CONDITIONS 10
    #define MAX_EFFECTS 10
    #define MAX_LEGAL_MOVES 28
    #define NUMBER_OF_SQUARES 64

    #define MIN_WHITE_MAN_INDEX 0
    #define MAX_WHITE_MAN_INDEX 5
    #define MIN_BLACK_MAN_INDEX 6
    #define MAX_BLACK_MAN_INDEX 11

    typedef unsigned long long int bitboard;

    typedef enum {
        white_pawn, white_rook, white_knight, white_bishop, white_queen, white_king,
        black_pawn, black_rook, black_knight, black_bishop, black_queen, black_king,
        white_enpassant, black_enpassant,
        white_castling, black_castling,
        white_control, black_control,
    } square_datum;
    
    typedef struct {
        bool must_be_empty; /* if 1, the square must be empty for a move to be legal; if 0 the square must be filled */
        int data_count;
        square_datum square_data[NUMBER_OF_SQUARE_DATA_TYPES]; /* the pieces that the square must not contain any of / must contain one of */
        bitboard squares; /* a bit with a a value of 1 indicates that a square must be checked */
    } condition;

    typedef struct{
        bool add_piece; /* 1 if a piece is being added, 0 if it is being removed */
        int data_count;
        square_datum square_data[NUMBER_OF_SQUARE_DATA_TYPES];
        bitboard squares;
    } effect;

    typedef struct {
        int condition_count;
        int effect_count;
        condition conditions[MAX_CONDITIONS];
        effect effects[MAX_EFFECTS];
    } move;

    typedef struct {
        int move_count;
        move moveset[MAX_LEGAL_MOVES];
    } moveset;

    void generate_bitboards(moveset * legal_moves);

#endif