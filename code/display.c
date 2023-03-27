#include <stdio.h>

#include "display.h"

#define A8 FIRST_SQUARE

#define DARK_SQUARE_COLOUR  "\e[48;5;0m"
#define LIGHT_SQUARE_COLOUR "\e[48;5;8m"
#define BLACK_CHESSMAN_COLOUR "\e[38;5;11m"
#define WHITE_CHESSMAN_COLOUR "\e[38;5;14m"
#define RESET_COLOUR "\e[0m"

const char chessman_characters[] = "PRNBQKprnbqk";
const char chessman_display_characters[] = "oRNBQK";

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
	for(; fen[i] != ' '; i++)
	{
		next_character = fen[i];

		for(int cm = 0; cm < NUMBER_OF_CHESSMEN; cm++)
			if(chessman_characters[cm] == next_character)
				position->bitboards[cm] |= square;

		square >>=
			('1' <= next_character && next_character <= '8')?
				next_character - '0':
				next_character == '/' ? 0 : 1;
	}

	/* side to move */
	for(i++; fen[i] != ' '; i++)
	{
		position->player_to_move = fen[i] == 'w';
	}

	/* castling */
	for(i++; fen[i] != ' '; i++)
	{
		switch(fen[i])
		{
			case 'K': position->bitboards[WHITE_CASTLING] |= WHITE_CASTLE_KINGSIDE_ENDSQUARE;  break;
			case 'Q': position->bitboards[WHITE_CASTLING] |= WHITE_CASTLE_QUEENSIDE_ENDSQUARE; break;
			case 'k': position->bitboards[BLACK_CASTLING] |= BLACK_CASTLE_KINGSIDE_ENDSQUARE;  break;
			case 'q': position->bitboards[BLACK_CASTLING] |= BLACK_CASTLE_QUEENSIDE_ENDSQUARE; break;
			default: break;
		}
	}

	/* en passant */
	for(i++; fen[i] != ' '; i++);

	/* half moves */
	for(i++; fen[i] != ' '; i++);

	/* full moves */
	for(i++; fen[i]; i++);

	for(enum square_datum chessman = WHITE_PAWN; chessman <= BLACK_KING; chessman++)
	{
		if(chessman <= WHITE_KING)
			position->bitboards[WHITE_CHESSMEN] |= position->bitboards[chessman];
		else
			position->bitboards[BLACK_CHESSMEN] |= position->bitboards[chessman];
	}

	position->bitboards[CHESSMEN] = position->bitboards[WHITE_CHESSMEN] | position->bitboards[BLACK_CHESSMEN];
}

void print_position(struct position position)
{
	int i = 0;
	char chessman[3] = "  ";
	for(type_bitboard square = A8; square; square >>= 1, i++)
	{
		chessman[0] = ' ';
		printf((i+i/8)% 2 ? DARK_SQUARE_COLOUR : LIGHT_SQUARE_COLOUR );
		for(enum square_datum cm = 0; cm < NUMBER_OF_CHESSMEN; cm++)
		{
			if(position.bitboards[cm] & square)
			{
				printf(cm <= WHITE_KING ? WHITE_CHESSMAN_COLOUR : BLACK_CHESSMAN_COLOUR);
				chessman[0] = chessman_display_characters[cm % CHESSMEN_PER_SIDE];
			}
		}
		printf(chessman);
		if(!((i+1) % 8)) printf("\n");
	}
	printf(RESET_COLOUR);
}
