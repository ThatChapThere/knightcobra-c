#ifndef POSITION_H
#define POSITION_H

#include "move_bitboards.h"

#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
//#define STARTING_FEN "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR b KQkq - 0 1"
//#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/7P/PPPPPPP1/RNBQKBNR w KQkq - 0 1"
//#define STARTING_FEN "r6r/8/8/8/8/8/8/R6R w KQkq - 0 1"

enum player {BLACK_PLAYER, WHITE_PLAYER};

struct position
{
	enum player player_to_move;
	int fifty_move_counter;
	type_bitboard bitboards[NUMBER_OF_SQUARE_DATA_TYPES];
};

void set_position_from_fen(struct position *current_position, char *fen);
void copy_position_to(struct position position_to_copy, struct position *position_to_copy_to);
void print_position(struct position position);

#endif
