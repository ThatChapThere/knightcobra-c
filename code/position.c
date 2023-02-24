#include <stdio.h>
#include "position.h"
#include "move_bitboards.h"

#define A8 FIRST_SQUARE

const char chessman_characters[] = "PRNBQKprnbqk";

void copy_position_to(struct position position_to_copy, struct position *position_to_copy_to)
{
	position_to_copy_to->fifty_move_counter = position_to_copy.fifty_move_counter;
	position_to_copy_to->player_to_move = position_to_copy.player_to_move;

	for(int i = 0; i < NUMBER_OF_SQUARE_DATA_TYPES; i++)
		position_to_copy_to->bitboards[i] = position_to_copy.bitboards[i];
}

void set_position_from_fen(struct position *position, char *fen)
{
	for(int cm = 0; cm < NUMBER_OF_SQUARE_DATA_TYPES; cm++)
		position->bitboards[cm] = 0ull;

	int i = 0;
	int square_offset;
	type_bitboard square = A8;
	char next_character = fen[0];

	/* the positions of the chessmen */
	for(; next_character != ' '; i++)
	{
		next_character = fen[i];

		for(int cm = 0; cm < NUMBER_OF_CHESSMEN; cm++)
			if(chessman_characters[cm] == next_character)
				position->bitboards[cm] |= square;

		square >>= 
			('0' < next_character && next_character < '9') ?
				next_character - '0' :
			next_character == '/' ?
				0 : 1;
	}

	/* side to move */
	for(; fen[i] != ' '; i++)
		position->player_to_move = fen[i] == 'w';

	for(enum square_datum chessman = WHITE_PAWN; chessman <= BLACK_KING; chessman++)
		if(chessman <= WHITE_KING)
			position->bitboards[WHITE_CHESSMEN] |= position->bitboards[chessman];
		else
			position->bitboards[BLACK_CHESSMEN] |= position->bitboards[chessman];
	
	position->bitboards[CHESSMEN] = position->bitboards[WHITE_CHESSMEN] | position->bitboards[BLACK_CHESSMEN];
	return;

	/* castling */
	for(; next_character != ' '; i++);

	/* en passant */
	for(; next_character != ' '; i++);

	/* half moves */
	for(; next_character != ' '; i++);

	/* full moves */
	for(; next_character; i++);
}

void print_position(struct position position)
{
	char output[73];
	output[72] = '\0';
	for(int i = 0; i < 72; i++)
	{
		output[i] = (i + 1) % 9 ?
			'_' : '\n';
	}
	int i = 0;
	int square_filled = 0, error = 0;
	for(type_bitboard square = A8; square; square >>= 1, i++)
	{
		/* set the default to a ? if there is a "chessman" set here */
		if(position.bitboards[CHESSMEN] & square)
		{
			output[i + i / 8] = '?';
		}
		square_filled = 0;
		for(enum square_datum cm = 0; cm < NUMBER_OF_CHESSMEN; cm++)
		{
			if(position.bitboards[cm] & square)
			{
				output[i + i / 8] = square_filled ? '2' : chessman_characters[cm];
				if(square_filled) error = 1;
				square_filled = 1;
			}
		}
	}
	printf("%s", output);
	if(error) printf("!!!!!!!!\n");
}
