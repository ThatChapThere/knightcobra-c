#ifndef _MOVE_BITBOARDS_H_
#define _MOVE_BITBOARDS_H_

#define NUMBER_OF_CHESSMEN 12
#define NUMBER_OF_SQUARE_DATA_TYPES 18 /* all 12 pieces, plus castling, en passant and control for both sides */
#define MAX_MOVE_CONDITIONS 10
#define MAX_MOVE_EFFECTS 10
#define MAX_LEGAL_MOVES 28
#define NUMBER_OF_SQUARES 64

#define MIN_WHITE_MAN_INDEX 0
#define MAX_WHITE_MAN_INDEX 5
#define MIN_BLACK_MAN_INDEX 6
#define MAX_BLACK_MAN_INDEX 11

typedef unsigned long int bitboard;

typedef enum {
    white_pawn, white_rook, white_knight, white_bishop, white_queen, white_king,
    black_pawn, black_rook, black_knight, black_bishop, black_queen, black_king,
    white_enpassant, black_enpassant,
    white_castling, black_castling,
    white_control, black_control,
} square_data_type;

typedef struct {
    _Bool must_be_empty; /* if 1, the square must be empty for a move to be legal; if 0 the square must be filled */
    int data_count;
    square_data_type square_data[NUMBER_OF_SQUARE_DATA_TYPES]; /* the pieces that the square must not contain any of / must contain one of */
    bitboard squares; /* a bit with a a value of 1 indicates that a square must be checked */
} move_condition;

typedef struct{
    _Bool add_piece; /* 1 if a piece is being added, 0 if it is being removed */
    int data_count;
    square_data_type square_data[NUMBER_OF_SQUARE_DATA_TYPES];
    bitboard squares;
} move_effect;

typedef struct {
    int move_condition_count;
    int move_effect_count;
    move_condition move_conditions[MAX_MOVE_CONDITIONS];
    move_effect move_effects[MAX_MOVE_EFFECTS];
} move;

typedef struct {
    int move_count;
    move moves[MAX_LEGAL_MOVES];
} moves;

extern void generate_bitboards(moves * legal_moves);

#endif