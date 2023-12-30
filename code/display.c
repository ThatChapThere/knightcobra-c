#include <stdio.h>

#include "display.h"

#define DARK_SQUARE_COLOUR  "\e[48;5;0m"
#define LIGHT_SQUARE_COLOUR "\e[48;5;8m"
#define BLACK_CHESSMAN_COLOUR "\e[38;5;11m"
#define WHITE_CHESSMAN_COLOUR "\e[38;5;14m"
#define RESET_COLOUR "\e[0m"

const char chessman_display_characters[] = "oRNBQK";

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
