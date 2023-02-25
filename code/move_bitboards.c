#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "move_bitboards.h"

#define BOARD_WIDTH 8
#define NUMBER_OF_LINE_PIECES 6
#define NUMBER_OF_STRAIGHT_PIECES 4
#define NUMBER_OF_DIAGONAL_PIECES 4
#define LINE_MOVE_DIRECTIONS 4

#define pawn_on_start_row(pawn, r) (is_white(pawn) ? r == 6 : r == 1)
#define pawn_about_to_promote(pawn, r) (is_white(pawn) ? r == 1 : r == 6)
#define pawn_row_invalid(r) (r == 0 || r == 7)
#define can_enpassant(pawn, r) (is_white(pawn) ? r == 3 : r == 4)

#define is_white(chessman) (chessman <= MAX_WHITE_MAN_INDEX)

enum line_move_direction {BOARDFILE, BOARDRANK, BACKSLASH_DIAGONAL, FORWARDSLASH_DIAGONAL};
enum castling_side {QUEENSIDE, KINGSIDE};

struct condition *get_next_condition(struct move *move);
struct effect *get_next_effect(struct move *move);
struct move *get_next_move_of_moveset(struct moveset *moveset);
enum square_datum own_chessmen(enum square_datum chessman);
enum square_datum opponent_control(enum square_datum chessman);
enum square_datum opponent_chessmen(enum square_datum chessman);
enum square_datum own_castling_blockers(enum square_datum king);
enum square_datum *get_next_datum_from_condition_or_effect(struct condition *condition_or_effect);
type_bitboard get_castling_unattacked_squares(enum square_datum king, enum castling_side side);
type_bitboard get_castling_empty_squares(enum square_datum king, enum castling_side side);
type_bitboard get_bit_from_line_move(int f, int r, int distance, enum line_move_direction line_move_direction);
struct moveset *get_moveset_from_coordinates_and_chessman(struct moveset *legal_moves, int f, int r, enum square_datum chessman);
void add_move_effects(struct move *move, type_bitboard squares, enum square_datum square_datum, bool adds);

/* line piece indices */
const enum square_datum line_pieces[] = {
	WHITE_ROOK, WHITE_BISHOP, WHITE_QUEEN,
	BLACK_ROOK, BLACK_BISHOP, BLACK_QUEEN
};

void generate_bitboards(struct moveset *legal_moves)
{
	for(int f = 0; f < BOARD_WIDTH; f++) /* a-h */
	for(int r = 0; r < BOARD_WIDTH; r++) /* 8-1 */
	{
		type_bitboard startsquare = get_bit_from_coords(f, r);

		generate_line_piece_bitboards (legal_moves, f, r, startsquare);
		generate_knight_bitboards	 (legal_moves, f, r, startsquare);
		generate_king_bitboards	   (legal_moves, f, r, startsquare);
		generate_pawn_bitboards	   (legal_moves, f, r, startsquare);
	}
}

void generate_line_piece_bitboards(struct moveset *legal_moves, int f, int r, type_bitboard startsquare)
{
	for(int lp = 0; lp < NUMBER_OF_LINE_PIECES; lp++)
	{
		enum square_datum line_piece = line_pieces[lp];

		/* get the set of moves from square (f, r) for piece "line_piece" */
		struct moveset *moveset = get_moveset_from_coordinates_and_chessman(legal_moves, f, r, line_piece);

		/* loop through directions */
		for(
			enum line_move_direction line_move_direction = 0;
			line_move_direction < LINE_MOVE_DIRECTIONS;
			line_move_direction++
		){
			/* rooks can't move diagonally, vice versa for bishops */
			bool is_diagonal = (bool) (line_move_direction & BACKSLASH_DIAGONAL & FORWARDSLASH_DIAGONAL);
			if(
				( is_diagonal && is_an_example_of_chessman(ROOK,   line_piece)) ||
				(!is_diagonal && is_an_example_of_chessman(BISHOP, line_piece))
			) continue;

			/* loop through end places */
			for(int end = 0; end < BOARD_WIDTH; end++)
			{
				/* the rank is used to indicate the start, unless it's a move along a rank */
				int start = line_move_direction == BOARDRANK ? f : r;
				int move_distance = end - start;
				int direction = abs(move_distance) / move_distance;

				/* moves cannot start and end on the same square */
				/* or leave the board */
				if(!move_distance) continue;
				if(!get_bit_from_line_move(f, r, move_distance, line_move_direction)) continue;

				/* add a new move */
				struct move *move = get_next_move_of_moveset(moveset);

				/* find intermediate squares */
				type_bitboard intermediate_squares = 0;

				/*
				 * loop through intermediate squares
				 * start by moving one square
				 * don't go the full way, because captures are possible
				 * add the current intermediate square to the intermediate squares
				 */
				int distance;
				for(
					distance = direction;
					distance != move_distance;
					distance += direction
				) intermediate_squares |= get_bit_from_line_move(f, r, distance, line_move_direction);

				remove_datum_from_squares(move, startsquare, line_piece);
				add_intermediate_squares_to_move(move, intermediate_squares);
				add_end_square_to_move(move, get_bit_from_line_move(f, r, distance, line_move_direction), line_piece);

				/* make castling impossible when a rook moves away from its start square */
				if(is_an_example_of_chessman(ROOK, line_piece))
				{
					if(is_white(line_piece))
					{
						if(startsquare == WHITE_CASTLE_KINGSIDE_ROOKSQUARE)
							remove_datum_from_squares(move, WHITE_CASTLE_KINGSIDE_ENDSQUARE, WHITE_CASTLING);

						if(startsquare == WHITE_CASTLE_QUEENSIDE_ROOKSQUARE)
							remove_datum_from_squares(move, WHITE_CASTLE_QUEENSIDE_ENDSQUARE, WHITE_CASTLING);
					/* black rooks */
					}else{
						if(startsquare == BLACK_CASTLE_KINGSIDE_ROOKSQUARE)
							remove_datum_from_squares(move, BLACK_CASTLE_KINGSIDE_ENDSQUARE, WHITE_CASTLING);

						if(startsquare == BLACK_CASTLE_QUEENSIDE_ROOKSQUARE)
							remove_datum_from_squares(move, BLACK_CASTLE_QUEENSIDE_ENDSQUARE, WHITE_CASTLING);
					}
				}
			}
		}
	}
}

void generate_knight_bitboards(struct moveset *legal_moves, int f, int r, type_bitboard startsquare)
{
	for(enum square_datum knight = WHITE_KNIGHT; knight <= BLACK_KNIGHT; knight += CHESSMEN_PER_SIDE)
	{
		struct moveset *moveset = get_moveset_from_coordinates_and_chessman(legal_moves, f, r, knight);
		int is_to_right, is_down, is_mostly_file;
		for(int knight_move = 0; knight_move < 8; knight_move++)
		{
			is_to_right	= knight_move & 1;
			is_down		= knight_move & 2;
			is_mostly_file = knight_move & 4;
			type_bitboard endsquare = get_bit_from_coords(
				f + ((is_to_right ? 1 : -1) * (is_mostly_file ? 2 : 1)),
				r + ((is_down	 ? 1 : -1) * (is_mostly_file ? 1 : 2))
			);
			if(endsquare)
			{
				struct move *move = get_next_move_of_moveset(moveset);
				remove_datum_from_squares(move, startsquare, knight);
				add_end_square_to_move(move, endsquare, knight);
			}
		}
	}
}

void generate_king_bitboards(struct moveset *legal_moves, int f, int r, type_bitboard startsquare)
{
	for(enum square_datum king = WHITE_KING; king <= BLACK_KING; king += CHESSMEN_PER_SIDE)
	{
		struct moveset *moveset = get_moveset_from_coordinates_and_chessman(legal_moves, f, r, king);

		int file_movement, rank_movement;

		/* regular king moves */
		for(int kingmove = 0; kingmove < 9; kingmove++)
		{
			file_movement = (kingmove % 3) - 1;
			rank_movement = (kingmove / 3) - 1;

			if( !(file_movement || rank_movement) ) continue;

			type_bitboard endsquare = get_bit_from_coords(
				f + file_movement,
				r + rank_movement
			);

			if(endsquare)
			{
				struct move *move = get_next_move_of_moveset(moveset);

				/* move king */
				remove_datum_from_squares(move, startsquare, king);
				add_end_square_to_move(move, endsquare, king);
				
				/* can no longer castle */
				if(startsquare == WHITE_KINGSQUARE && king == WHITE_KING)
				{
					remove_datum_from_squares(move, WHITE_CASTLE_KINGSIDE_ENDSQUARE, WHITE_CASTLING);
					remove_datum_from_squares(move, WHITE_CASTLE_QUEENSIDE_ENDSQUARE, WHITE_CASTLING);
				}
				else if(startsquare == BLACK_KINGSQUARE && king == BLACK_KING)
				{
					remove_datum_from_squares(move, BLACK_CASTLE_KINGSIDE_ENDSQUARE, BLACK_CASTLING);
					remove_datum_from_squares(move, BLACK_CASTLE_QUEENSIDE_ENDSQUARE, BLACK_CASTLING);
				}

				/* cannot move into check */
				/* you can't legally move into check by any means regardless */
				/* this line exists purely for performance */
				squares_must_be_free_of(move, endsquare, opponent_control(king));
			}
		}

		/* castling */
		if(
			(startsquare == WHITE_KINGSQUARE && king == WHITE_KING) ||
			(startsquare == BLACK_KINGSQUARE && king == BLACK_KING)
		){
			for(enum castling_side side = QUEENSIDE; side <= KINGSIDE; side++)
			{
				struct move *castling_move = get_next_move_of_moveset(moveset);

				enum square_datum rook = is_white(king) ? WHITE_ROOK : BLACK_ROOK;

				const type_bitboard endsquare =
					is_white(king) ?
						side == KINGSIDE ?
							WHITE_CASTLE_KINGSIDE_ENDSQUARE:
							WHITE_CASTLE_QUEENSIDE_ENDSQUARE:
						side == KINGSIDE ?
							BLACK_CASTLE_KINGSIDE_ENDSQUARE:
							BLACK_CASTLE_QUEENSIDE_ENDSQUARE;

				const type_bitboard rook_startsquare =
					is_white(king) ?
						side == KINGSIDE ?
							WHITE_CASTLE_KINGSIDE_ROOKSQUARE:
							WHITE_CASTLE_QUEENSIDE_ROOKSQUARE:
						side == KINGSIDE ?
							BLACK_CASTLE_KINGSIDE_ROOKSQUARE:
							BLACK_CASTLE_QUEENSIDE_ROOKSQUARE;

				const type_bitboard rook_endsquare =
					side == KINGSIDE ?
						endsquare << 1:
						endsquare >> 1;

				const enum square_datum own_castling_possible =
					is_white(king) ? WHITE_CASTLING : BLACK_CASTLING;

				/* move king */
				remove_datum_from_squares(castling_move, startsquare, king);
				add_datum_to_squares(castling_move, endsquare, king);

				/* move rook */
				remove_datum_from_squares(castling_move, rook_startsquare, rook);
				add_datum_to_squares(castling_move, rook_endsquare, rook);

				type_bitboard castling_unattacked_squares = get_castling_unattacked_squares(king, side);
				type_bitboard castling_empty_squares = get_castling_empty_squares(king, side);

				/* cannot castle out of or through check */
				squares_must_be_free_of(castling_move, castling_unattacked_squares, opponent_control(king));
				squares_must_be_free_of(castling_move, castling_empty_squares, CHESSMEN);

				/* must be allowed to castle */
				squares_must_have(castling_move, endsquare, own_castling_possible);

				puts("castling");
				/* cannot castle twice */
				if(king == WHITE_KING)
				{
					remove_datum_from_squares(castling_move, WHITE_CASTLE_KINGSIDE_ENDSQUARE, WHITE_CASTLING);
					remove_datum_from_squares(castling_move, WHITE_CASTLE_QUEENSIDE_ENDSQUARE, WHITE_CASTLING);
				}else{
					remove_datum_from_squares(castling_move, BLACK_CASTLE_KINGSIDE_ENDSQUARE, BLACK_CASTLING);
					remove_datum_from_squares(castling_move, BLACK_CASTLE_QUEENSIDE_ENDSQUARE, BLACK_CASTLING);
				}
			}
		}
	}
}

void generate_pawn_bitboards(struct moveset *legal_moves, int f, int r, type_bitboard startsquare)
{
	for(enum square_datum pawn = WHITE_PAWN; pawn <= BLACK_PAWN; pawn += CHESSMEN_PER_SIDE)
	{
		if(pawn_row_invalid(r)) break;

		struct moveset *moveset = get_moveset_from_coordinates_and_chessman(legal_moves, f, r, pawn);

		const int rank_direction = is_white(pawn) ? -1 : 1;

		/* non promotion */
		if(!pawn_about_to_promote(pawn, r))
		{
			/* single step pawn move */
			type_bitboard single_step_endsquare = get_bit_from_coords(f, r + rank_direction);
			struct move *single_step = get_next_move_of_moveset(moveset);

			/* move pawn */
			remove_datum_from_squares(single_step, startsquare, pawn);
			add_datum_to_squares(single_step, single_step_endsquare, pawn);

			/* require emptiness at endsquare */
			add_intermediate_squares_to_move(single_step, single_step_endsquare);

			/* double step pawn move */
			if(pawn_on_start_row(pawn, r))
			{
				type_bitboard double_step_endsquare = get_bit_from_coords(f, r + rank_direction * 2);
				struct move *double_step = get_next_move_of_moveset(moveset);
				enum square_datum own_enpassant = 
					is_white(pawn) ?
						WHITE_ENPASSANT:
						BLACK_ENPASSANT;

				/* move pawn */
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
				type_bitboard endsquare = get_bit_from_coords(f + capture_direction, r + rank_direction);
				if(!endsquare) continue;

				/* normal captures */
				struct move *pawn_capture = get_next_move_of_moveset(moveset);

				/* pick up pawn */
				remove_datum_from_squares(pawn_capture, startsquare, pawn);
				/* capture and place */
				add_end_square_to_move(pawn_capture, endsquare, pawn);
				/* must be a capture */
				squares_must_have(pawn_capture, endsquare, opponent_chessmen(pawn));

				/* en passant */
				if(!can_enpassant(pawn, r)) continue;

				enum square_datum opponent_enpassant = 
					is_white(pawn) ?
						BLACK_ENPASSANT:
						WHITE_ENPASSANT;
				
				enum square_datum opponent_pawn = 
					is_white(pawn) ?
						BLACK_PAWN:
						BLACK_PAWN;
				
				type_bitboard enpassant_capture_square = get_bit_from_coords(f + capture_direction, r);
				
				struct move *enpassant = get_next_move_of_moveset(moveset);

				/* move pawn */
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
			for(enum square_datum p = WHITE_ROOK; p <= WHITE_QUEEN; p++)
			{
				enum square_datum promotion_piece = p + (is_white(pawn) ? 0 : CHESSMEN_PER_SIDE);

				/* single step pawn move */
				type_bitboard single_step_endsquare = get_bit_from_coords(f, r + rank_direction);
				struct move *single_step = get_next_move_of_moveset(moveset);

				/* promote pawn */
				remove_datum_from_squares(single_step, startsquare, pawn);
				add_datum_to_squares(single_step, single_step_endsquare, promotion_piece);

				/* require emptiness at endsquare */
				add_intermediate_squares_to_move(single_step, single_step_endsquare);

				/* captures */
				for(int capture_direction = -1; capture_direction <= 1; capture_direction += 2)
				{
					type_bitboard endsquare = get_bit_from_coords(f + capture_direction, r + rank_direction);
					if(!endsquare) continue;

						/* normal captures */
					struct move *pawn_capture = get_next_move_of_moveset(moveset);

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

void add_single_effect(struct move *move, type_bitboard squares, enum square_datum square_datum, bool adds)
{
	struct effect *effect = get_next_effect(move);
	effect->datum = square_datum;
	effect->adds = adds;
	effect->squares = squares;
}

void add_move_effects(struct move *move, type_bitboard squares, enum square_datum square_datum, bool adds)
{
	add_single_effect(move, squares, square_datum, adds);
	/* if we move a specific piece */
	if(is_chessman(square_datum))
	{
		enum square_datum side = own_chessmen(square_datum);
		add_single_effect(move, squares, side, adds);
		add_single_effect(move, squares, CHESSMEN, adds);
		/* these will add or remove the fact that castling is blocked, but only on the relevant ranks
		 * the limiting squares are h2 and a7 respectively */
	}
	/* if we mean to remove any piece of a given color, ie. a capture
	 *
	 * we don't apply an effect to the CHESSMAN bitboard, since captures always involve one chessman replacing
	 * another
	 * the only exception to this is en passant, where we remove pawns only anyway 
	 * this is a bit confusing, but basically if you remove a pawn, the above code is called, and
	 * so the CHESSMAN bitboard is corrected */
	if(square_datum == WHITE_CHESSMEN || square_datum == BLACK_CHESSMEN)
	{
		add_single_effect(move, squares, square_datum, adds);
		enum square_datum chessman;
		for(enum chessman type_of_chessman = 0; type_of_chessman < CHESSMEN_PER_SIDE; type_of_chessman++)
		{
			chessman = type_of_chessman + (square_datum == WHITE_CHESSMEN ? MIN_WHITE_MAN_INDEX : MIN_BLACK_MAN_INDEX);
			add_single_effect(move, squares, chessman, false);
		}
	}
}

void add_single_condition(struct move *move, type_bitboard squares, enum square_datum chessman, bool must_contain)
{
	struct condition *condition = get_next_condition(move);
	condition->datum = chessman;
	condition->must_contain = must_contain;
	condition->squares = squares;
}

void remove_datum_from_squares(struct move *move, type_bitboard squares, enum square_datum chessman)
{
	add_move_effects(move, squares, chessman, false);
}

void add_datum_to_squares(struct move *move, type_bitboard squares, enum square_datum chessman)
{
	add_move_effects(move, squares, chessman, true);
}

void squares_must_be_free_of(struct move *move, type_bitboard squares, enum square_datum chessman)
{
	add_single_condition(move, squares, chessman, false);
}

void squares_must_have(struct move *move, type_bitboard squares, enum square_datum chessman)
{
	add_single_condition(move, squares, chessman, true);
}

/* add a set of squares that must be completely empty */
void add_intermediate_squares_to_move(struct move *move, type_bitboard squares)
{
	squares_must_be_free_of(move, squares, CHESSMEN);
}

/* add a set of squares where:
	 * opponent pieces will be removed 
	 * the current piece will be added 
	 * no pieces of the same colour may be present */
void add_end_square_to_move(struct move *move, type_bitboard squares, enum square_datum chessman)
{
	add_datum_to_squares(move, squares, chessman);
	remove_datum_from_squares(move, squares, opponent_chessmen(chessman));
	squares_must_be_free_of(move, squares, own_chessmen(chessman));
}

struct effect *get_next_effect(struct move * move){
	return &move->effects[ move->effect_count++ ];
}

struct condition *get_next_condition(struct move *move){
	return &move->conditions[ move->condition_count++ ];
}

bool is_an_example_of_chessman(enum chessman that_kind_of_chessman, enum square_datum chessman)
{
	return
	chessman					 == that_kind_of_chessman ||
	chessman - CHESSMEN_PER_SIDE == that_kind_of_chessman;
}

enum square_datum own_chessmen(enum square_datum chessman){
	return is_white(chessman) ? WHITE_CHESSMEN : BLACK_CHESSMEN;
}

enum square_datum opponent_control(enum square_datum chessman){
	return is_white(chessman) ? WHITE_CONTROL : BLACK_CONTROL;
}

enum square_datum opponent_chessmen(enum square_datum chessman){
	return is_white(chessman) ? BLACK_CHESSMEN : WHITE_CHESSMEN;
}

type_bitboard get_castling_unattacked_squares(enum square_datum king, enum castling_side side)
{
	int castling_direction = side == QUEENSIDE ? -1 : 1;
	enum square_datum king_square = is_white(king) ? WHITE_KINGSQUARE : BLACK_KINGSQUARE;
	return king_square | (king_square >> castling_direction) | (king_square >> (2 * castling_direction));
}

type_bitboard get_castling_empty_squares(enum square_datum king, enum castling_side side)
{
	type_bitboard king_square = is_white(king) ? WHITE_KINGSQUARE : BLACK_KINGSQUARE;
	if(side == QUEENSIDE)
		return (king_square << 1) | (king_square << 2) | (king_square << 3);
	else
		return (king_square >> 1) | (king_square >> 2);
}

/* return a bitboard with a 1 at the end of a line move */
type_bitboard get_bit_from_line_move(int f, int r, int distance, enum line_move_direction line_move_direction)
{
	switch (line_move_direction)
	{
		/* move along a file */
		case BOARDFILE:
			return get_bit_from_coords(f, r + distance);
		
		/* move along a rank */
		case BOARDRANK:
			return get_bit_from_coords(f + distance, r);

		/* backslash diagonal move */
		case BACKSLASH_DIAGONAL:
			return get_bit_from_coords(f + distance, r + distance);

		/* forward slash diagonal move */
		case FORWARDSLASH_DIAGONAL:
			/* we have to try to flip across both axes because the start square is the pivot point */
			/* "distance" has a range dependent on the start rank, it's not 0-7 like "end" */
			return get_bit_from_coords(f + distance, r - distance)?
				get_bit_from_coords(f + distance, r - distance):
				get_bit_from_coords(f - distance, r + distance);
	}
	return 0;
}

/* return a bitboard with a 1 at specified coordinates */
type_bitboard get_bit_from_coords(int f, int r)
{
	/* if the move is off of the board
	 * ie. either number has a bit out of the 0-7 range */
	if((f|r) & ~(1|2|4)) return 0;

	/* 
	 * eg. for h1: 
	 * f and r are 7
	 * 7 + 7 * 8 = 63
	 * correctly returns 1ull - the last bit
	 */
	int square_index = f + 8 * r;
	return FIRST_SQUARE >> square_index;
}

struct move *get_next_move_of_moveset(struct moveset *moveset){
	return &moveset->moves[moveset->move_count++];
}

struct moveset *get_moveset_from_coordinates_and_chessman(struct moveset * legal_moves, int f, int r, enum square_datum chessman)
{
	return &legal_moves[
		chessman * NUMBER_OF_SQUARES +
		f + (8 * r)
	];
}
