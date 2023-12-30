#ifndef FEN_H
#define FEN_H

#include "position.h"

#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
//#define STARTING_FEN "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR b KQkq - 0 1"
//#define STARTING_FEN "rnbqkbnr/pppppppp/8/8/8/7P/PPPPPPP1/RNBQKBNR w KQkq - 0 1"
//#define STARTING_FEN "r6r/8/8/8/8/8/8/R6R w KQkq - 0 1"

void set_position_from_fen(struct position *current_position, char *fen);

#endif
