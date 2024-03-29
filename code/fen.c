#include "fen.h"

const char chessman_fen_characters[] = "PRNBQKprnbqk";

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
			if(chessman_fen_characters[cm] == next_character)
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
