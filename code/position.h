#ifndef POSITION_H
#define POSITION_H

#include "rules.h"

#define A8 FIRST_SQUARE

enum player {BLACK_PLAYER, WHITE_PLAYER};

struct position
{
	enum player player_to_move;
	int fifty_move_counter;
	type_bitboard bitboards[NUMBER_OF_SQUARE_DATA_TYPES];
};

void copy_position_to(struct position position_to_copy, struct position *position_to_copy_to);

#endif
