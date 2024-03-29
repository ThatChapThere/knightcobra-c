#ifndef MOVE_BITBOARDS_H
#define MOVE_BITBOARDS_H

#include <stdbool.h>
#include <stdint.h>

#define NUMBER_OF_CHESSMEN 12
#define CHESSMEN_PER_SIDE 6
#define MAX_CONDITIONS 10
/* format: chessmen + general
 * pick piece up = 1 + 2
 * place piece down = 1 + 2
 * capture any opponent pieces = 6 + 2
 * change whether or not castling is possible = 1
 * total = 15 */
#define MAX_EFFECTS 24 // TODO: determine what number this should be
#define MAX_MOVES_PER_CHESSMAN 27
#define NUMBER_OF_SQUARES 64

#define MIN_WHITE_MAN_INDEX WHITE_PAWN
#define MAX_WHITE_MAN_INDEX WHITE_KING
#define MIN_BLACK_MAN_INDEX BLACK_PAWN
#define MAX_BLACK_MAN_INDEX BLACK_KING

#define WHITE_KINGSQUARE get_bit_from_coords(4, 7)
#define WHITE_CASTLE_QUEENSIDE_ENDSQUARE get_bit_from_coords(2, 7)
#define WHITE_CASTLE_QUEENSIDE_ROOKSQUARE get_bit_from_coords(0, 7)
#define WHITE_CASTLE_KINGSIDE_ENDSQUARE get_bit_from_coords(6, 7)
#define WHITE_CASTLE_KINGSIDE_ROOKSQUARE get_bit_from_coords(7, 7)
#define BLACK_KINGSQUARE get_bit_from_coords(4, 0)
#define BLACK_CASTLE_QUEENSIDE_ENDSQUARE get_bit_from_coords(2, 0)
#define BLACK_CASTLE_QUEENSIDE_ROOKSQUARE get_bit_from_coords(0, 0)
#define BLACK_CASTLE_KINGSIDE_ENDSQUARE get_bit_from_coords(6, 0)
#define BLACK_CASTLE_KINGSIDE_ROOKSQUARE get_bit_from_coords(7, 0)

#define is_chessman(sq_dtm) (sq_dtm <= BLACK_KING)
#define is_white(chessman) (chessman <= MAX_WHITE_MAN_INDEX)

#define FIRST_SQUARE (((uint64_t) 1) << 63)

typedef uint64_t type_bitboard;

enum square_datum
{
	WHITE_PAWN, WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING,
	BLACK_PAWN, BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING,
	WHITE_ENPASSANT, BLACK_ENPASSANT,
	WHITE_CASTLING, BLACK_CASTLING,
	WHITE_CONTROL, BLACK_CONTROL,
	WHITE_CHESSMEN, BLACK_CHESSMEN,
	CHESSMEN,
	NUMBER_OF_SQUARE_DATA_TYPES 
};

enum chessman {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING};

struct condition
{
	bool must_contain;
	int data_count;
	enum square_datum datum; /* the datum that the square must (not) contain */
	type_bitboard squares; /* a bit with a a value of 1 indicates that a square must be checked */
};

struct effect
{
	bool adds; /* if 0, the square must be empty for a move to be legal; if 1 the square must be filled */
	int data_count;
	enum square_datum datum; /* the datum that the square must (not) contain */
	type_bitboard squares; /* a bit with a a value of 1 indicates that a square must be checked */
};

struct move
{
	int condition_count;
	int effect_count;
	struct condition conditions[MAX_CONDITIONS];
	struct effect effects[MAX_EFFECTS];
};

struct moveset
{
	int move_count;
	struct move moves[MAX_MOVES_PER_CHESSMAN];
};

/* functions */
void generate_bitboards(struct moveset *legal_moves);

void generate_line_piece_bitboards (struct moveset *legal_moves, int f, int r, type_bitboard startsquare);
void generate_knight_bitboards     (struct moveset *legal_moves, int f, int r, type_bitboard startsquare);
void generate_king_bitboards       (struct moveset *legal_moves, int f, int r, type_bitboard startsquare);
void generate_pawn_bitboards       (struct moveset *legal_moves, int f, int r, type_bitboard startsquare);

void add_single_effect(struct move *move, type_bitboard squares, enum square_datum chessman, bool must_contain);
void add_datum_to_squares(struct move *move, type_bitboard squares, enum square_datum chessman);
void remove_datum_from_squares(struct move *move, type_bitboard squares, enum square_datum chessman);

void add_single_condition(struct move *move, type_bitboard squares, enum square_datum chessman, bool adds);
void squares_must_have(struct move *move, type_bitboard squares, enum square_datum chessman);
void squares_must_be_free_of(struct move *move, type_bitboard squares, enum square_datum chessman);

void add_intermediate_squares_to_move(struct move *move, type_bitboard squares);
void add_end_square_to_move(struct move *move, type_bitboard squares, enum square_datum chessman);
void add_through_squares_to_castling(struct move *move, type_bitboard squares, enum square_datum king);

bool is_an_example_of_chessman(enum chessman kind_of_chessman, enum square_datum chessman);

type_bitboard get_bit_from_coords(int f, int r);

#endif
